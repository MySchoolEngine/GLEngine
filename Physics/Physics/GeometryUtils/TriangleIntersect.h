#pragma once

#include <Physics/Primitives/Ray.h>

#include <glm/glm.hpp>


namespace GLEngine::Physics {

template <class triangleDef> float TraingleRayIntersect(const triangleDef& vertices, const Primitives::S_Ray& ray, glm::vec2* uv = nullptr)
{
	// Möller–Trumbore intersection algorithm
	const float EPSILON = 0.0000001f;
	using namespace glm;
	const vec3	edge1 = vertices[1] - vertices[0];
	const vec3	edge2 = vertices[2] - vertices[0];
	const vec3	h	  = cross(ray.direction, edge2);
	const float det	  = dot(edge1, h);
	if (det > -EPSILON && det < EPSILON)
		return -1.f; // This ray is parallel to this triangle.
	const float invDet = 1.0f / det;
	const vec3 s = ray.origin - vertices[0];
	const float u = invDet * dot(s, h);
	if (u < 0.0f || u > 1.0f)
		return -1.f;
	const vec3 q = cross(s, edge1);
	const float v = invDet * dot(ray.direction, q);
	if (v < 0.0f || u + v > 1.0f)
		return -1.f;
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = invDet * dot(edge2, q);
	if (uv)
		*uv = {u, v};
	if (t > EPSILON) // ray intersection
	{
		return t;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return -1.f;
}

} // namespace GLEngine::Physics