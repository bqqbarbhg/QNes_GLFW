#include "glm/glm.hpp"

#define DOT(a, b) (((a).x * (b).x) + ((a).y * (b)).y + ((a).z * (b).z))
const double epsilon = 4.37114e-05;
#define EPSILON epsilon

inline glm::vec3 getNearTri(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	using namespace glm;

	vec3 D0 = p - a;
	vec3 E1 = b - a;
	vec3 E2 = c - a;

	float dot11 = DOT(E1, E1);
	float dot12 = DOT(E1, E2);
	float dot22 = DOT(E2, E2);
	float dot1d = DOT(E1, D0);
	float dot2d = DOT(E2, D0);
	float dotdd = DOT(D0, D0);

	float s = dot1d * dot22 - dot2d * dot12;
	float t = dot2d * dot11 - dot1d * dot12;
	float d = dot11 * dot22 - dot12 * dot12;

	if(dot1d <= 0 && dot2d <= 0)
		return a;

	if(s <= 0 && dot2d >= 0 && dot2d <= dot22)
		return a + E2 * (dot2d / dot22);

	if (t <= 0 && dot1d >= 0 && dot1d <= dot11)
		return a + E1 * (dot1d / dot11);

	if (s >= 0 && t >= 0 && s + t <= d)
	{
		float dr = 1.0f / d;
		return a + E1 * (s * dr) + E2 * (t * dr);
	}

	float u12_num = dot11 + dot2d - dot12 - dot1d;
	float u12_den = dot11 + dot22 - (2.0f * dot12);

	if (u12_num <= 0)
		return b;
	if (u12_num >= u12_den)
		return c;
	
	return b + (c - b) * (u12_num / u12_den);
}

inline bool rayIntersectTri(const glm::vec3 &origin, const glm::vec3 &dir, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
	float *t, float *u, float *v, const bool cull)
{
	using namespace glm;

	vec3 e1 = v1 - v0;
	vec3 e2 = v2 - v0;

	vec3 tvec, pvec, qvec;
	float det, inv_det;

	pvec = cross(dir, e2);

	det = DOT(e1, pvec);

	if(cull)
	{
		if(det < EPSILON)
			return false;

		tvec = origin - v0;

		*u = DOT(tvec, pvec);
		if(*u < 0.0f || *u > det)
			return false;

		qvec = cross(tvec, e1);

		*v = DOT(dir, qvec);
		if(*v < 0.0f || *u + *v > det)
			return false;

		*t = DOT(e2, qvec);

		inv_det = 1.0f / det;

		*t *= inv_det;
		*u *= inv_det;
		*v *= inv_det;
	}
	else
	{
		if(det > -EPSILON && det < EPSILON)
			return false;

		inv_det = 1.0f / det;
		tvec = origin - v0;
		*u = DOT(tvec, pvec) * inv_det;
		if(*u < 0.0f || *u > 1.0f)
			return false;

		qvec = cross(tvec, e1);

		*v = DOT(dir, qvec) * inv_det;
		if(*v < 0.0f || *u + *v > 1.0f)
			return false;

		*t = DOT(e2, qvec) * inv_det;
	}
	return true;
}

#undef DOT