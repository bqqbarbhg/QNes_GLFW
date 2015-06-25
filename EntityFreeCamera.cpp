
#include "EntityFreeCamera.h"
#include "Scene.h"
#include "glm/gtc/matrix_transform.hpp"
#include "transform3.h"

using namespace glm;

vec3 pos, dir, up;

vec2 oldMouse = vec2(0, 0);
vec2 cre = vec2(0);

EntityFreeCamera::EntityFreeCamera()
{
	pos = vec3(0, 0, 0);
	up = vec3(0, 1, 0);
	dir = vec3(0, 0, 1);
}

EntityFreeCamera::EntityFreeCamera(vec3 p)
{
	pos = p;
	up = vec3(0, 1, 0);
	dir = vec3(0, 0, 1);
}

void EntityFreeCamera::update()
{
	int mx, my;
	glfwGetMousePos(&mx, &my);
	vec2 mpos(mx, my);

	up = vec3(0, 1, 0);
	vec3 right = cross(dir, up);
	vec3 m = vec3(0);

	if(glfwGetKey('W'))
		m += dir;
	if(glfwGetKey('A'))
		m -= right;
	if(glfwGetKey('S'))
		m -= dir;
	if(glfwGetKey('D'))
		m += right;

	
	if(length(m) > 0)
	{
		m = normalize(m);
		float speed = 4.0f;
		if(glfwGetKey(GLFW_KEY_LSHIFT))
			speed = 1.0f;
		pos += m * speed * dt;
	}
	
	if(glfwGetMouseButton(0))
	{
		cre += (oldMouse - mpos) * 0.25f;
		mat4 rot = mat4();
		
		rot = rotate(rot, cre.x,  vec3(0, 1, 0)) * rotate(rot, -cre.y, vec3(1, 0, 0));
		dir = transform3(rot, vec3(0, 0, 1));
	}

	camera->pos = pos;
	camera->dir = dir;
	camera->up = up;

	oldMouse = mpos;
}
