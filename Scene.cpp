#include "Scene.h"
#include "Camera.h"
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

Scene::Scene()
{
	entity_ptr = new Entity*[2048];
	entity_c = 0;
	eid = 0;
	static_coll_c = 0;
	callback_add_entity = NULL;
	gravity = vec3(0.0f, -5.0f, 0.0f);
}

void Scene::add(Entity* e)
{
	entity_ptr[entity_c++] = e;
	e->init();
	e->id = eid++;
	e->last_update = 0;
	if(callback_add_entity)
		callback_add_entity->invoke(e);
}

void Scene::add_static_id(Entity* e)
{
	entity_ptr[entity_c++] = e;
	e->init();
	e->last_update = 0;
	if(callback_add_entity)
		callback_add_entity->invoke(e);
}

void Scene::addAddCallback(EntityCallbackBase* ecb)
{
	if(callback_add_entity)
		delete callback_add_entity;
	callback_add_entity = ecb;
}

void Scene::update()
{
	Entity* e;
	for(Entity** ent = entity_ptr; ent < entity_ptr + entity_c; ent++)
	{
		e = *ent;

		e->update();

		if(e->dead)
		{
			delete e;
			memcpy(ent, ent + 1, (int)(entity_ptr + entity_c - ent));
		}
	}
}

void Scene::draw()
{
	camera->apply();

	Entity* e;
	for(Entity** ent = entity_ptr; ent < entity_ptr + entity_c; ent++)
	{
		e = *ent;

		e->draw();
	}

/*	glLineWidth(4.0f);
	glPointSize(10.0f);
	glEnable(GL_POLYGON_OFFSET_LINE);
	//glEnable(GL_POLYGON_OFFSET_POINT);
	glPolygonOffset(-0.05f, 0.1f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	StaticCollisionMesh* m;
	for(StaticCollisionMesh** msh = static_coll_ptr; msh < static_coll_ptr + static_coll_c; msh++)
	{
		m = *msh;
		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

		glVertexPointer(3, GL_FLOAT, 0, m->vert_ptr);

		glDrawElements(GL_TRIANGLES, m->index_c, GL_UNSIGNED_INT, m->index_ptr);
		
	}

	vec3* pts = new vec3[50000];

	int q = getPoints(camera->pos - vec3(0.0f, 0.4f, 0.0f), 0.6f, pts);

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR);

	

	glBegin(GL_POINTS);
	for(vec3* p = pts; p < pts + q; p++)
	{
		
		vec3 qq = *p;
		glColor3f(length(qq - camera->pos) < 2.0f, 0.0f, 0.0f);
		glVertex3f(qq.x, qq.y, qq.z);
	}
	glEnd();

	glDisable(GL_COLOR);
	glEnable(GL_LIGHTING);

	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);

	delete[] pts;
	*/
	

}

int Scene::getPoints(const vec3& v, const float max_dist, vec3* vp, const int m)
{
	int pts = 0;
	vec3* vpp = vp;
	for(StaticCollisionMesh** smc = static_coll_ptr; smc < static_coll_ptr + static_coll_c; smc++)
	{
		if(pts >= m)
			return pts;

		int sz = (**smc).getPoints(v, max_dist, vpp, m);
		pts += sz;
		vpp += sz;
	}
	return pts;
}

bool Scene::rayCastStatic(const glm::vec3& origin, const glm::vec3& dir,
									float *t, float *u, float *v,  const bool cull)
{

	float _t, _u, _v;
	float __vt, __vu, __vv;
	float *__t = &__vt, *__u = &__vu, *__v = &__vv;

	bool hit = false;

	for(StaticCollisionMesh** smc = static_coll_ptr; smc < static_coll_ptr + static_coll_c; smc++)
	{
		
		if((**smc).rayCast(origin, dir, __t, __u, __v, cull))
		{
			if(!hit || (__vt < _t))
			{
				_t = __vt; _u = __vu; _v = __vv;
				hit = true;
			}
		}
	}

	if(hit)
	{
		*t = _t; *u = _u; *v = _v;
	}

	return hit;
}

void Scene::send(VirtualConnection* v)
{
	Entity* e;
	for(Entity** ent = entity_ptr; ent < entity_ptr + entity_c; ent++)
	{
		e = *ent;
		e->send(v);
	}
}

Entity* Scene::entityById(int id)
{
	for(Entity** ent = entity_ptr; ent < entity_ptr + entity_c; ent++)
	{
		if((*ent)->id == id)
			return *ent;
	}
	return NULL;
}

bool Scene::existsById(int id)
{
	for(Entity** ent = entity_ptr; ent < entity_ptr + entity_c; ent++)
	{
		if((*ent)->id == id)
			return true;
	}
	return false;
}

void Scene::receive(VirtualConnection* v)
{
	Packet packet;
	Address sender;
	
	while (net_socket->receive(sender, packet))
	{
 		if(packet.readHeader())
		{
			char op = packet.readChar();

			if(op == 'e')
			{
				entity_ptr[packet.readInt()]->receive(v, packet);
			}
		}
	}
}

StaticCollisionMesh** Scene::expandStaticCollision(int n)
{
	int oldsize = static_coll_c;

	if(static_coll_c != 0)
	{
		StaticCollisionMesh** old = static_coll_ptr;
		static_coll_ptr = (StaticCollisionMesh**)malloc(sizeof(StaticCollisionMesh*) * (static_coll_c + n));
		memcpy(static_coll_ptr, old, sizeof(StaticCollisionMesh*) * static_coll_c);
		free(old);
	}
	else
	{
		static_coll_ptr = (StaticCollisionMesh**)malloc(sizeof(StaticCollisionMesh*) * n);
	}

	static_coll_c += n;

	return static_coll_ptr + oldsize;

}

void Scene::write(VirtualConnection* v)
{
	for(Entity** ent = entity_ptr; ent < entity_ptr + entity_c; ent++)
	{
		(*ent)->write(v);
	}
}
