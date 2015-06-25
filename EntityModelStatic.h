#pragma once

#include "Entity.h"
#include "Model.h"
#include "glm/glm.hpp"
#include "virtualconnection.h"

class EntityModelStatic : public Entity
{
public:
	const char* src;
	EntityModelStatic(Model* model);
	Model* model;
	glm::mat4 transform;

	virtual void draw();
	virtual void init();

	virtual void write(VirtualConnection* v);
	static EntityModelStatic* create(VirtualConnection* v, Packet& p);

	~EntityModelStatic();
};