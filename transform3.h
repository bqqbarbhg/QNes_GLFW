#pragma once

#include "glm/glm.hpp"

const glm::vec3		UP			=	glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3		DOWN		=	glm::vec3(0.0f, -1.0f, 0.0f);

const glm::vec3		RIGHT		=	glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3		LEFT		=	glm::vec3(-1.0f, 0.0f, 0.0f);

const glm::vec3		FORWARD		=	glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3		BACKWARD	=	glm::vec3(0.0f, 0.0f, -1.0f);


#define DOT(a, b) (((a).x * (b).x) + ((a).y * (b)).y + ((a).z * (b).z))

inline glm::vec3& transform3(const glm::mat4x4& m, const glm::vec3& v)
{
	return glm::vec3(
		  m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0],
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1],
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2]);
}