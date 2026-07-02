#pragma once
#include <xmmintrin.h>

namespace GLEngine::Physics::Primitives {
struct S_Ray {
	// can't be constexpr as division by zero is valid here
	S_Ray(const glm::vec3& origin, const glm::vec3& direction)
		: origin(origin)
		, direction(direction)
	{
		__m128 dir = _mm_set_ps(0.f, direction.z, direction.y, direction.x);
		invDirection = _mm_div_ps(_mm_set_ps(0, 1, 1, 1), dir);
	}
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
	__m128 invDirection;
};
} // namespace GLEngine::Physics::Primitives