#pragma once

#include "glm/glm.hpp"
#include "MeshPart.h"

class Mesh
{
public:
	Mesh();
	
	glm::mat4 transform;
	


	int part_c;
	MeshPart* part_ptr;

	char* name;

	void draw();
	void draw(glm::mat4 transform);

};