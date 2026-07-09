#pragma once

#include <Physics/Primitives/Ray.h>

#include <Utils/SSE/SSEUtils.h>

#include <glm/glm.hpp>

#include <array>

namespace GLEngine::Physics {

template <class TriangleDef> float TriangleRayIntersect(const TriangleDef& vertices, const Primitives::S_Ray& ray, glm::vec2* barycentric = nullptr)
{
	// Moller-Trumbore intersection algorithm
	constexpr float EPSILON = 0.0000001f;
	using namespace glm;
	const vec3	edge1 = vertices[1] - vertices[0]; // TODO: precalculate? and measure
	const vec3	edge2 = vertices[2] - vertices[0]; // TODO: precalculate? and measure
	const vec3	h	  = cross(ray.direction, edge2);
	const float det	  = dot(edge1, h);
	if (det > -EPSILON && det < EPSILON)
		return std::numeric_limits<float>::infinity(); // This ray is parallel to this triangle.
	const float invDet = 1.0f / det;
	const vec3	s	   = ray.origin - vertices[0];
	const float u	   = invDet * dot(s, h);
	if (u < 0.0f || u > 1.0f)
		return std::numeric_limits<float>::infinity();
	const vec3	q = cross(s, edge1);
	const float v = invDet * dot(ray.direction, q);
	if (v < 0.0f || u + v > 1.0f)
		return std::numeric_limits<float>::infinity();
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = invDet * dot(edge2, q);
	if (barycentric)
		*barycentric = {u, v};
	if (t > EPSILON) // ray intersection
	{
		return t;
	}
	return std::numeric_limits<float>::infinity();
}

template <class TriangleDef> float TriangleRayIntersect(const TriangleDef& vertices, const Primitives::S_SSERay& ray, glm::vec2* barycentric = nullptr)
{
	using namespace ::Utils::SSE;
	// Moller-Trumbore intersection algorithm
	constexpr float EPSILON = 0.0000001f;
	const Vec3		vert[3]{Vec3(vertices[0]), Vec3(vertices[1]), Vec3(vertices[2])};

	const Vec3	edge1 = vert[1] - vert[0]; // TODO: precalculate? and measure
	const Vec3	edge2 = vert[2] - vert[0]; // TODO: precalculate? and measure
	const Vec3	h	  = ray.direction.Cross(edge2);
	const float det	  = edge1.Dot(h);
	if (det > -EPSILON && det < EPSILON)
		return std::numeric_limits<float>::infinity(); // This ray is parallel to this triangle.
	const float invDet = 1.0f / det;
	const Vec3	s	   = ray.origin - vert[0];
	const float u	   = invDet * s.Dot(h);
	if (u < 0.0f || u > 1.0f)
		return std::numeric_limits<float>::infinity();
	const Vec3	q = s.Cross(edge1);
	const float v = invDet * ray.direction.Dot(q);
	if (v < 0.0f || u + v > 1.0f)
		return std::numeric_limits<float>::infinity();
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = invDet * edge2.Dot(q);
	if (barycentric)
		*barycentric = {u, v};
	if (t > EPSILON) // ray intersection
	{
		return t;
	}
	return std::numeric_limits<float>::infinity();
}

} // namespace GLEngine::Physics