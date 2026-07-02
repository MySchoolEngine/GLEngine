#pragma once

#include <Utils/SSE/SSEUtils.h>

#include <xmmintrin.h>

namespace GLEngine::Physics::Primitives {
struct S_Ray {
	// can't be constexpr as division by zero is valid here
	S_Ray(const glm::vec3& origin, const glm::vec3& direction)
		: origin(origin)
		, direction(direction)
	{
		__m128 dir	 = ::Utils::SSE::ToSSE(direction);
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
	__m128	  invDirection;
};

struct S_SSERay {
	// can't be constexpr as division by zero is valid here
	S_SSERay(const glm::vec3& origin, const glm::vec3& direction)
		: origin(origin)
		, direction(direction)
		, invDirection(::Utils::SSE::SSEVec3(1.f, 1.f, 1.f) / this->direction)
	{
	}

	explicit S_SSERay(const S_Ray& ray)
		: S_SSERay(ray.origin, ray.direction)
	{
	}

	::Utils::SSE::SSEVec3 origin;
	::Utils::SSE::SSEVec3 direction;
	::Utils::SSE::SSEVec3 invDirection;
};
} // namespace GLEngine::Physics::Primitives