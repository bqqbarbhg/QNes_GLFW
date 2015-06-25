#pragma once

#include <GL/glew.h>
#include <GL/glfw.h>
#include "glm/glm.hpp"
#include "VertexElement.h"

class MeshPart
{

public:
	MeshPart();
	
	int tex_c;
	GLuint* tex_ptr;

	int vert_c;
	float* vert_ptr;

	int index_c;
	bool index_16;
	unsigned short* index_ptr_16;
	unsigned int* index_ptr_32;

	int vert_stride;

	int elem_c;
	VertexElement* elem_ptr;

	VertexElement* getElement(char type, int index);

	void draw();

	void createBuffers();

protected:
	GLuint vbo_vert;
	GLuint vbo_index;

};