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
		return -(glm::dot(glm::vec4(ray.origin, 1.0f), noraml) - originOffset) / (glm::dot(ray.direction, glm::vec3(noraml.x, noraml.y, noraml.z)));
	}
};
}