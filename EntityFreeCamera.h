#pragma once

#include "Camera.h"
#include "Entity.h"

class EntityFreeCamera : public Entity
{
public:
	EntityFreeCamera();
	EntityFreeCamera(glm::vec3 pos);
	virtual void update();

private:
	inline void initn();
};