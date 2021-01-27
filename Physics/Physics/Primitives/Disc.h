#pragma once

#include <Physics/Primitives/Ray.h>

#include <Physics/Primitives/Intersectable.h>

#include <Physics/Primitives/Plane.h>

#include <glm/glm.hpp>

namespace GLEngine::Physics::Primitives {
struct S_Ray;

struct S_Disc : public T_Intersectable<S_Disc> {
	constexpr S_Disc(glm::vec3 _normal, glm::vec3 _center, float _radius)
		: plane(_normal, _center)
		, radius(_radius)
	{}
	S_Plane	plane;
	float		radius;
	[[nodiscard]] inline constexpr float	IntersectImpl(const S_Ray& ray) const
	{
		const auto t = plane.IntersectImpl(ray);
		if (t > 0.0)
		{
			const auto p = ray.origin + ray.direction * t;
			const auto v = p - plane.origin;
			const auto d2 = glm::dot(v, v);
			if (std::sqrt(d2) <= radius) 
				return t;
		}

		return -1.f;
	}
};
}