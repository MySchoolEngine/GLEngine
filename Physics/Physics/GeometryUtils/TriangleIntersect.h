#pragma once

#include <Physics/Primitives/Ray.h>

#include <glm/glm.hpp>


namespace GLEngine::Physics {

template <class triangleDef> float TraingleRayIntersect(const triangleDef& vertices, const Primitives::S_Ray& ray)
{
	// Möller–Trumbore intersection algorithm
	const float EPSILON = 0.0000001f;
	using namespace glm;
	vec3  edge1, edge2, h, s, q;
	float a, f, u, v;
	edge1 = vertices[1] - vertices[0];
	edge2 = vertices[2] - vertices[0];
	h	  = cross(ray.direction, edge2);
	a	  = dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return false; // This ray is parallel to this triangle.
	f = 1.0f / a;
	s = ray.origin - vertices[0];
	u = f * dot(s, h);
	if (u < 0.0f || u > 1.0f)
		return false;
	q = cross(s, edge1);
	v = f * dot(ray.direction, q);
	if (v < 0.0f || u + v > 1.0f)
		return false;
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * dot(edge2, q);
	if (t > EPSILON) // ray intersection
	{
		return t;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return -1.f;
}

}