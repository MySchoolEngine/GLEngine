#pragma once

#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Ray.h>

namespace GLEngine::Physics::Primitives {
struct S_Ray;

struct S_Rectangle {
	constexpr S_Rectangle(const S_Plane& _plane, glm::vec3 _right, glm::vec3 _up, float _halfWidth, float _halfHeight)
		: plane(_plane)
		, right(_right)
		, up(_up)
		, halfWidth(_halfWidth)
		, halfHeight(_halfHeight)
	{
	}
	S_Plane	  plane; // origin = center, normal = facing direction
	glm::vec3 right; // normalized in-plane axis
	glm::vec3 up;	 // normalized in-plane axis
	float	  halfWidth;
	float	  halfHeight;

	[[nodiscard]] inline float IntersectImpl(const S_Ray& ray, const float tMax) const
	{
		const auto t = plane.IntersectImpl(ray, tMax);
		if (t < 0.0)
		{
			return -1.f;
		}

		const auto p = ray.origin + ray.direction * t - plane.origin;
		if (std::abs(glm::dot(p, right)) <= halfWidth && std::abs(glm::dot(p, up)) <= halfHeight)
			return t;
		return -1.f;
	}
};
} // namespace GLEngine::Physics::Primitives
