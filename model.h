#pragma once

#include "Mesh.h"
#include <GL/glew.h>
#include <GL/glfw.h>

class Model
{
public:
	
	Model();

	static Model* load(const char* name);
	static Model* loadNoTex(const char* name);

	const char* src;

	GLuint* tex_ptr;

	int mesh_c;
	Mesh* mesh_ptr;

	void draw();
	void draw(glm::mat4 transform);
};

