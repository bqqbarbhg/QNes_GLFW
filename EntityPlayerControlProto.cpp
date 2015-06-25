#if 0

#include "EntityPlayerControlProto.h"
#include "Camera.h"
#include "Scene.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "transform3.h"
#include "servercontrol.h"

using namespace glm;


inline void EntityPlayerControlProto::initn()
{
	cpts = new vec3[1024];
}

EntityPlayerControlProto::~EntityPlayerControlProto()
{
	delete [] cpts;
}

EntityPlayerControlProto::EntityPlayerControlProto()
{
	pos = vec3(0, 0, 0);
	up = vec3(0, 1, 0);
	dir = vec3(0, 0, 1);

	initn();
}

EntityPlayerControlProto::EntityPlayerControlProto(vec3 p)
{
	pos = p;
	up = vec3(0, 1, 0);
	dir = vec3(0, 0, 1);

	initn();
}

EntityPlayerControlProto::EntityPlayerControlProto(vec3 p, Address a)
{
	pos = p;
	up = vec3(0, 1, 0);
	dir = vec3(0, 0, 1);

	owner = a;

	initn();
}

void EntityPlayerControlProto::write(VirtualConnection* v)
{
	Packet* p = v->packet_manager->startDlen(NET_OP_ADD_ENTITY);

	p->writeInt(id);
	p->writeChar(ENTITY_TYPE_ENTITY_PLAYER_CONTROL_PROTO);
	p->writeVec3(pos);
	p->writeVec3(vel);
	p->writeChar(v->address == owner);

	v->packet_manager->endDlen();
}

EntityPlayerControlProto* EntityPlayerControlProto::create(VirtualConnection* v, Packet& p)
{
	EntityPlayerControlProto* e = new EntityPlayerControlProto();

	e->pos = p.readVec3();
	e->vel = p.readVec3();
	e->owned = p.readChar();

	if(e->owned)
		v->state->player = e;

	return e;
}

void EntityPlayerControlProto::send(VirtualConnection* v)
{
	Packet* p = v->packet_manager->startDlen(NET_OP_UPDATE_ENTITY);

	p->writeInt(id);
	p->writeVec3(pos);
	p->writeVec3(vel);

	v->packet_manager->endDlen();
}

void EntityPlayerControlProto::receive(VirtualConnection* v, Packet& p)
{
	pos = p.readVec3();
	vel = p.readVec3();
}

void EntityPlayerControlProto::update()
{
	int mx, my;
	glfwGetMousePos(&mx, &my);

	//glfwDisable(GLFW_MOUSE_CURSOR);

	vec2 mpos(mx, my);

	up = vec3(0, 1, 0);
	fdir = normalize(vec3(dir.x, 0, dir.z));
	
	right = cross(fdir, up);
	vec3 m = vec3(0);

	if(owned)
	{
		if(server)
		{
			char wasd = state->pstate.wasd;
			if(wasd & 1)
				m += fdir;
			if((wasd >> 1) & 1)
				m -= right;
			if((wasd >> 2) & 1)
				m -= fdir;
			if((wasd >> 3) & 1)
				m += right;
		}
		else if(keyb_input)
		{
			if(glfwGetKey('W'))
				m += fdir;
			if(glfwGetKey('A'))
				m -= right;
			if(glfwGetKey('S'))
				m -= fdir;
			if(glfwGetKey('D'))
				m += right;
		}
	}

	if(length(m) > 0)
	{
		m = normalize(m);
		float speed = 10.0f;
		vel += m * speed * dt;
	}
	
	pos += vel * dt * 3.0f;
	if(!onGround)
		vel += vec3(0, -dt, 0) * 2.1f;
	else
		vel.y *= 1.0f - dt * 12.0f;
	
	if(onGround && owned)
	{

		if(server ? ((state->pstate.wasd >> 4) & 1) : (glfwGetKey(GLFW_KEY_SPACE) && keyb_input))
		{
			vel.y = 1.5f;
			pos.y += 0.08f;
		}

	}

	vel.x *= 1 - dt * 8; vel.z *= 1 - dt * 8;

	

	if(keyb_input)
	{
		if(server)
			cre = vec2(state->pstate.mx, state->pstate.my);
		else
			cre += (vec2(700, 500) - mpos) * vec2(0.3f, 0.4f);
	}

	mat4 rot = mat4();
	
	cre.y = clamp(cre.y, -80.0f, 80.0f);

	rot = rotate(rot, cre.x,  vec3(0, 1, 0)) * rotate(rot, -cre.y, vec3(1, 0, 0));
	dir = transform3(rot, vec3(0, 0, 1));
	

	float rad = 0.4f;

	int cpc = scene->getPoints(pos, rad + 1.0f, cpts);
	vec3 d;
	for(vec3* v = cpts; v < cpts + cpc; v++)
	{
		d = pos - *v;
		float s = d.x * d.x + d.y * d.y + d.z * d.z;
		if(s < rad * rad)
		{
			d = normalize(d);
			pos += (rad - sqrt(s)) * d;
			

			mat4 m = glm::gtc::matrix_transform::lookAt(
				vec3(0.0f),
				d,
				abs(d.y) > 0.5f ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f)
				);

			vel = transform3(m, vel);
			vel.z = 0;
			vel = transform3(inverse(m), vel);

		}
	}

	float u, v, t;

	onGround = false;
	if(scene->rayCastStatic(pos, vec3(0.0f, -1.0f, 0.0f), &t, &u, &v, false))
	{
		float snap = 0.12f;

		onGround = t < rad + snap;
		if(onGround)
		{
			pos.y += (rad + snap - 0.05f) - t;
			vel.y = 0.0f;
		}
	}

	if(owned)
	{
		camera->pos = pos + vec3(0.0f, 0.4f, 0.0f);
		camera->dir = dir;
		camera->up = up;
	}

	if(owned && !glfwGetKey('F') && keyb_input)
		glfwSetMousePos(700, 500);

}

void EntityPlayerControlProto::draw()
{
	if(!owned)
	{
		vec3 p = pos - vec3(0.0f, 0.4f, 0.0f);

		glBegin(GL_TRIANGLES);
		glVertex3fv(value_ptr(p));
		glVertex3fv(value_ptr(p + up + fdir * 0.2f));
		glVertex3fv(value_ptr(p + up - fdir * 0.2f));

		glVertex3fv(value_ptr(p));
		glVertex3fv(value_ptr(p + up + right * 0.2f));
		glVertex3fv(value_ptr(p + up - right * 0.2f));
		glEnd();
	}
}

#endif