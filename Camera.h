#pragma once

#include <GL/glew.h>
#include <GL/glfw.h>
#include "glm/glm.hpp"

class Camera
{
public:
	glm::vec3 pos, dir, up;

	Camera()
		:pos(0, 0, 0), dir(0, 0, 1), up(0, 1, 0)
	{}

	void apply()
	{
		glm::vec3 t = pos + dir;

		gluLookAt(	pos.x,	pos.y,	pos.z,
					t.x,	t.y,	t.z,
					up.x,	up.y,	up.z	);
	}
};

extern Camera* camera;