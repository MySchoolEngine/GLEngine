#pragma once

#include <Physics/Primitives/Ray.h>

#include <Physics/Primitives/Intersectable.h>

#include <glm/glm.hpp>

namespace GLEngine::Physics::Primitives {
struct S_Ray;

struct S_Plane : public T_Intersectable<S_Plane> {
	constexpr S_Plane(glm::vec4 _normal, float offset)
		: normal(_normal)
		, originOffset(offset) {}
	glm::vec4	normal;
	float		originOffset;
	[[nodiscard]] inline float	IntersectImpl(const S_Ray& ray) const
	{
		return -(glm::dot(glm::vec4(ray.origin, 1.0f), normal) - originOffset) / (glm::dot(ray.direction, glm::vec3(normal.x, normal.y, normal.z)));
	}
};
}