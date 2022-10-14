#pragma once

#include <Physics/Primitives/Intersectable.h>
#include <Physics/Primitives/Ray.h>

#include <glm/glm.hpp>

namespace GLEngine::Physics::Primitives {
struct S_Ray;

struct S_Plane : public T_Intersectable<S_Plane> {
	constexpr S_Plane(glm::vec3 _normal, float offset)
		: normal(_normal)
		, origin(normal * offset)
		, twoSided(true)
	{
	}
	constexpr S_Plane(glm::vec3 _normal, glm::vec3 origin)
		: normal(_normal)
		, origin(origin)
		, twoSided(true)
	{
	}
	glm::vec3				   normal;
	glm::vec3				   origin;
	bool					   twoSided : 1;
	[[nodiscard]] inline float IntersectImpl(const S_Ray& ray) const
	{
		const auto useNormal = ((glm::dot(-ray.direction, normal) < 0 && twoSided) ? -normal : normal);
		if (glm::dot(-ray.direction, useNormal) < 0.0)
		{
			return -1;
		}

		return (glm::dot(origin - ray.origin, useNormal)) / (glm::dot(useNormal, ray.direction));
	}
};
} // namespace GLEngine::Physics::Primitives