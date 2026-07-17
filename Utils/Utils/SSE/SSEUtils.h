#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <xmmintrin.h>

namespace Utils::SSE {
inline __m128 ToSSE(const glm::vec3& vec)
{
	return _mm_set_ps(0.f, vec.z, vec.y, vec.x);
}
inline __m128 ToSSE(const glm::vec4& vec)
{
	return _mm_set_ps(vec.w, vec.z, vec.y, vec.x);
}
} // namespace Utils::SSE