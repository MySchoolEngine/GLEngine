#pragma once
#include <glm/vec3.hpp>

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

class alignas(16) SSEVec3 {
	__m128 data;

public:
	SSEVec3()
		: data(_mm_setzero_ps())
	{
	}

	explicit SSEVec3(const glm::vec3& vec)
		: data(ToSSE(vec))
	{
	}

	SSEVec3(float x, float y, float z)
		: data(_mm_set_ps(0.f, z, y, x))
	{
	}

	SSEVec3(__m128 x)
		: data(x)
	{
	}

	[[nodiscard]] SSEVec3 operator-(const SSEVec3& other) const { return _mm_sub_ps(data, other.data); }
	[[nodiscard]] SSEVec3 operator+(const SSEVec3& other) const { return _mm_add_ps(data, other.data); }
	[[nodiscard]] SSEVec3 operator*(const SSEVec3& other) const { return _mm_mul_ps(data, other.data); }
	[[nodiscard]] SSEVec3 operator/(const SSEVec3& other) const { return _mm_div_ps(data, other.data); }

	__m128 GetRaw() const { return data; }

	[[nodiscard]] SSEVec3 Cross(const SSEVec3& other) const
	{
		__m128 a = _mm_shuffle_ps(data, data, _MM_SHUFFLE(3, 0, 2, 1));
		__m128 b = _mm_shuffle_ps(other.data, other.data, _MM_SHUFFLE(3, 1, 0, 2));
		__m128 c = _mm_shuffle_ps(data, data, _MM_SHUFFLE(3, 1, 0, 2));
		__m128 d = _mm_shuffle_ps(other.data, other.data, _MM_SHUFFLE(3, 0, 2, 1));
		return _mm_sub_ps(_mm_mul_ps(a, b), _mm_mul_ps(c, d));
	}

	float Dot(const SSEVec3& other) const { return _mm_cvtss_f32(_mm_dp_ps(data, other.data, 0x71)); }
};
} // namespace Utils::SSE