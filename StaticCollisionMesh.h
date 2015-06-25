#pragma once
#include "glm/glm.hpp"

class StaticCollisionMesh
{
public:
	StaticCollisionMesh(unsigned int* iptr, glm::vec3* vptr, int index_count);
	~StaticCollisionMesh();

	glm::vec3* vert_ptr;
	
	unsigned int* index_ptr;
	int index_c;

	glm::vec3 getClosestPoint(const glm::vec3& p, const float max_dist);
	int getPoints(const glm::vec3& p, const float max_dist, glm::vec3* points, const int max_points);
	bool rayCast(const glm::vec3& origin, const glm::vec3& dir, float *t, float *u, float *v, const bool cull);
};