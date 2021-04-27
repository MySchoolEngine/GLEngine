#pragma once

#include <Physics/Primitives/Ray.h>

#include <glm/glm.hpp>

namespace GLEngine::Physics {
[[nodiscard]] float DistanceToRay(const glm::vec3& point, const Primitives::S_Ray& ray)
{
	const glm::vec3 Vector		  = point - ray.origin;
	const glm::vec3 LineDirection = ray.direction; // should be normalized

	// Project Vector to LineDirection to get the distance of point from ray.origin
	auto Distance = glm::dot(Vector, LineDirection);

	if (Distance <= 0.0f)
		return glm::distance(ray.origin, point);
	return glm::distance(point, ray.origin + ray.direction * Distance); // TODO optimise
}
} // namespace GLEngine::Physics