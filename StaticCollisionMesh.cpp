#pragma once

#include "StaticCollisionMesh.h"
#include "trianglecollision.h"

using namespace glm;

StaticCollisionMesh::StaticCollisionMesh(unsigned int* iptr, glm::vec3* vptr, int index_count)
{
	index_ptr = iptr;
	vert_ptr = vptr;
	index_c = index_count;
}

StaticCollisionMesh::~StaticCollisionMesh()
{
	delete [] vert_ptr;
	delete [] index_ptr;
}

vec3 StaticCollisionMesh::getClosestPoint(const vec3& p, const float md = 5000.0f)
{
	unsigned int* end = index_ptr + index_c;

	vec3 closest;
	float closest_dist = md * md;
	float dist;
	vec3 val;
	vec3 d;

	for(unsigned int* i = index_ptr; i < end; i+=3)
	{
		val = getNearTri(p, vert_ptr[index_ptr[0]], vert_ptr[index_ptr[1]], vert_ptr[index_ptr[2]]);
		d = val - p;	
		dist = d.x * d.x + d.y * d.y + d.z * d.z;

		if(dist < closest_dist)
		{
			closest = val;
			closest_dist = dist;
		}
	}

	return val;
}

int StaticCollisionMesh::getPoints(const glm::vec3& p, const float md, vec3* points, const int max_points)
{
	unsigned int* end = index_ptr + index_c;

	vec3* pptr = points;
	float dist = md * md;
	vec3 val;
	int pts = 0;
	vec3 d;

	for(unsigned int* i = index_ptr; i < end; i+=3)
	{
		val = getNearTri(p, vert_ptr[i[0]], vert_ptr[i[1]], vert_ptr[i[2]]);

		d = val - p;	

		if( d.x * d.x + d.y * d.y + d.z * d.z < dist)
		{
			if(pts >= max_points)
				return pts;

			*pptr++ = val;
			pts++;
		}
	}

	return pts;
}

bool StaticCollisionMesh::rayCast(const glm::vec3& origin, const glm::vec3& dir,
									float *t, float *u, float *v,  const bool cull)
{
	unsigned int* end = index_ptr + index_c;

	float _t, _u, _v;
	float __vt, __vu, __vv;
	float *__t = &__vt, *__u = &__vu, *__v = &__vv;

	bool hit = false;

	for(unsigned int* i = index_ptr; i < end; i+=3)
	{
		
		if(rayIntersectTri(origin, dir, vert_ptr[i[0]], vert_ptr[i[1]], vert_ptr[i[2]], __t, __u, __v, cull))
		{
			if(__vt >= 0 && (!hit || (__vt < _t)))
			{
				_t = __vt; _u = __vu; _v = __vv;
				hit = true;
			}
		}
	}

	if(hit)
	{
		*t = _t; *u = _u; *v = _v;
	}

	return hit;
}