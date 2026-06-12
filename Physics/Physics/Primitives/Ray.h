#pragma once

namespace GLEngine::Physics::Primitives {
struct S_Ray {

	/**
	 * @brief This function serves for moving the ray e.g. when the original one hit transparent
	 *        surface
	 *
	 * @param offset length to offset the ray origin
	 * @return
	 */
	S_Ray OffsetRay(float offset) const { return {origin + (direction * offset), direction}; }

	glm::vec3 origin;
	glm::vec3 direction;
};
} // namespace GLEngine::Physics::Primitives