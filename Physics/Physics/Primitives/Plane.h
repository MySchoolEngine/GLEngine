#pragma once

#include <Physics/Primitives/Ray.h>

#include <Physics/Primitives/Intersectable.h>

#include <glm/glm.hpp>

namespace GLEngine::Physics::Primitives {
struct S_Ray;

struct S_Plane : public T_Intersectable<S_Plane> {
	constexpr S_Plane(glm::vec3 _normal, float offset)
		: normal(_normal)
		, origin(normal* offset) {}
	constexpr S_Plane(glm::vec3 _normal, glm::vec3 origin)
		: normal(_normal)
		, origin(origin) {}
	glm::vec3	normal;
	glm::vec3	origin;
	[[nodiscard]] inline constexpr float	IntersectImpl(const S_Ray& ray) const
	{
		if (glm::dot(ray.direction, normal) > 0.0)
		{
			return -1;
		}

		return (glm::dot(origin - ray.origin, normal)) / (glm::dot(ray.direction, normal));
	}
};
}