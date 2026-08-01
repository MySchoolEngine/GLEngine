#pragma once

#include <Utils/SSE/SSEUtils.h>
#include <Utils/SSE/Vec3.h>

namespace GLEngine::Physics::Primitives {
struct S_SSERay;
struct S_Ray {
	// can't be constexpr as division by zero is valid here
	constexpr S_Ray(const glm::vec3& origin, const glm::vec3& direction)
		: origin(origin)
		, direction(direction)
	{
	}
	/**
	 * @brief This function serves for moving the ray e.g. when the original one hit transparent
	 *        surface
	 *
	 * @param offset length to offset the ray origin
	 * @return
	 */
	S_Ray OffsetRay(float offset) const { return {origin + (direction * offset), direction}; }

	void TransformRay(const glm::mat4& invSpaceMat)
	{
		origin	  = invSpaceMat * glm::vec4(origin, 1.f);
		direction = invSpaceMat * glm::vec4(direction, 0.f); // 0 to ignore translation
	}

	[[nodiscard]] S_Ray GetTransformedRay(const glm::mat4& invSpaceMat) const
	{
		S_Ray newRay = *this;
		newRay.TransformRay(invSpaceMat);
		return newRay;
	}

	glm::vec3 origin;
	glm::vec3 direction;
};

struct alignas(16) S_SSERay {
	// can't be constexpr as division by zero is valid here
	S_SSERay(const glm::vec3& origin, const glm::vec3& direction)
		: origin(origin)
		, direction(direction)
		, invDirection(::Utils::SSE::Vec3(1.f, 1.f, 1.f) / this->direction)
	{
	}

	explicit S_SSERay(const S_Ray& ray)
		: S_SSERay(ray.origin, ray.direction)
	{
	}

	explicit operator S_Ray() const { return S_Ray(static_cast<glm::vec3>(origin), static_cast<glm::vec3>(direction)); }

	::Utils::SSE::Vec3 origin;
	::Utils::SSE::Vec3 direction;
	::Utils::SSE::Vec3 invDirection;
};
} // namespace GLEngine::Physics::Primitives