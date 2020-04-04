#pragma once

#include <Physics/Primitives/Ray.h>

#include <glm/glm.hpp>

namespace GLEngine::Physics::Primitives {
struct S_Ray;

struct S_Plane {
	glm::vec4	noraml;
	float		originOffset;
	[[nodiscard]] inline float	Intersect(const S_Ray& ray) const
	{
		return -(glm::dot(ray.origin, noraml) - originOffset) / (glm::dot(ray.direction, noraml));
	}
};
}