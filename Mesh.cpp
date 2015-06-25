#include <GL/glew.h>
#include <GL/glfw.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "Mesh.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

Mesh::Mesh()
{

}

void Mesh::draw()
{
	glPushMatrix();

	glMultMatrixf(glm::value_ptr(transform));

	for(MeshPart* part = part_ptr; part < part_ptr + part_c; part++)
	{
		part->draw();
	}

	glPopMatrix();
} 

void Mesh::draw(mat4 t)
{
	glPushMatrix();

	glMultMatrixf(glm::value_ptr(t * transform));

	for(MeshPart* part = part_ptr; part < part_ptr + part_c; part++)
	{
		part->draw();
	}

	glPopMatrix();
} 