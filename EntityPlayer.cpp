#include "EntityPlayer.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "transform3.h"
#include "servercontrol.h"
#include "Camera.h"


using namespace glm;

EntityPlayer::EntityPlayer(PlayerControl* c)
{
	Entity::Entity();
	control = c;
}

EntityPlayer::EntityPlayer(PlayerControl* c, Address a)
{
	Entity::Entity();
	control = c;
	owner = a;
}

EntityPlayer::~EntityPlayer()
{
	delete control;
}

void EntityPlayer::update()
{
	control->update();

	printf("%d:\t%f\t%f\t", id, control->state->mx, control->state->my);

	if(owned && !server)
	{
		camera->pos = control->state->pos + vec3(0.0f, 0.4f, 0.0f);
		camera->dir = control->state->getDir();
		camera->up = UP;
	}
}

void EntityPlayer::draw()
{
	if(!owned)
	{
		vec3 p = control->state->pos - vec3(0.0f, 0.4f, 0.0f);

		vec3 fdir = control->state->getFDir();
		vec3 right = control->state->getRight();

		glBegin(GL_TRIANGLES);
		glVertex3fv(value_ptr(p));
		glVertex3fv(value_ptr(p + UP + fdir * 0.2f));
		glVertex3fv(value_ptr(p + UP - fdir * 0.2f));

		glVertex3fv(value_ptr(p));
		glVertex3fv(value_ptr(p + UP + right * 0.2f));
		glVertex3fv(value_ptr(p + UP - right * 0.2f));
		glEnd();
	}
}

void EntityPlayer::send(VirtualConnection* v)
{
	Packet* p = v->packet_manager->startDlen(NET_OP_UPDATE_ENTITY);

	p->writeInt(id);

	if(server && owner == v->address)
		static_cast<PlayerControlServer*>(control)->send_predicted(p);
	else
		control->send(p);

	v->packet_manager->endDlen();
}

void EntityPlayer::receive(VirtualConnection* v, Packet& p)
{
	control->receive(&p);
}

void EntityPlayer::write(VirtualConnection* v)
{
	Packet* p = v->packet_manager->startDlen(NET_OP_ADD_ENTITY);

	p->writeInt(id);
	p->writeChar(ENTITY_TYPE_ENTITY_PLAYER);

	p->writeChar(v->address == owner);
	
	control->send(p);

	v->packet_manager->endDlen();
}

EntityPlayer* EntityPlayer::create(VirtualConnection* v, Packet& p)
{
	EntityPlayer* e;

	char owned = p.readChar();
	

	if(owned)
	{
		e = new EntityPlayer(new PlayerControlClientPredicted());
		v->state->player = e->control;
	}
	else 
		e = new EntityPlayer(new PlayerControlClient());

	e->owned = owned;

	*e->control->state = PState(&p);

	p.dptr += PInput::packet_size;

	return e;
}