#include "transform3.h"

inline glm::vec3& transform3(const glm::mat4x4& m, const glm::vec3& v)
{
	return glm::vec3(
		  m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0],
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1],
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2]);
}