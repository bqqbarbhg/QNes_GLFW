#pragma once


#include <stdio.h>
#include "Entity.h"
#include "StaticCollisionMesh.h"
#include "callback.h"

class Scene
{
public:
	Scene();
	int entity_c;
	Entity** entity_ptr;

	glm::vec3 gravity;

	int static_coll_c;
	StaticCollisionMesh** static_coll_ptr;

	StaticCollisionMesh** expandStaticCollision(int n);

	void update();
	void draw();
	void add(Entity* e);
	void add_static_id(Entity* e);

	void addAddCallback(EntityCallbackBase* callback);

	void write(VirtualConnection* v);

	int getPoints(const glm::vec3& v, const float max_dist, glm::vec3* vp, const int max_points);
	bool rayCastStatic(const glm::vec3& origin, const glm::vec3& dir, float *t, float *u, float *v, const bool cull);

	Entity* entityById(int id);
	bool existsById(int id);

	void send(VirtualConnection* v);
	void receive(VirtualConnection* v);

private:
	EntityCallbackBase* callback_add_entity;
	int eid;
};

extern Scene* scene;
extern float dt, s_time;

