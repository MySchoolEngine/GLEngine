#pragma once

#include <Utils/SSE/SSEUtils.h>

#include <smmintrin.h>

namespace Utils::SSE {

class alignas(16) Vec3 {
	__m128 data;

public:
	Vec3()
		: data(_mm_setzero_ps())
	{
	}
	explicit Vec3(float val)
		: data(_mm_set_ps(0.f, val, val, val))
	{
	}

	explicit Vec3(const glm::vec3& vec)
		: data(ToSSE(vec))
	{
	}

	Vec3(float x, float y, float z)
		: data(_mm_set_ps(0.f, z, y, x))
	{
	}

	Vec3(__m128 x)
		: data(x)
	{
	}

	[[nodiscard]] Vec3 operator-(const Vec3& other) const { return _mm_sub_ps(data, other.data); }
	[[nodiscard]] Vec3 operator+(const Vec3& other) const { return _mm_add_ps(data, other.data); }
	[[nodiscard]] Vec3 operator*(const Vec3& other) const { return _mm_mul_ps(data, other.data); }
	[[nodiscard]] Vec3 operator/(const Vec3& other) const { return _mm_div_ps(data, other.data); }
	[[nodiscard]] Vec3 operator*(const float val) const { return _mm_mul_ps(data, _mm_set1_ps(val)); }

	[[nodiscard]] bool operator==(const Vec3& other) const
	{
		__m128 cmp	= _mm_cmpeq_ps(data, other.data); // 0xFFFFFFFF per lane if equal, 0 if not
		int	   mask = _mm_movemask_ps(cmp);			  // bit i = sign bit of lane i
		return (mask & 0x7) == 0x7;
	}

	static Vec3 min(const Vec3& a, const Vec3& b) { return _mm_min_ps(a.data, b.data); }
	static Vec3 max(const Vec3& a, const Vec3& b) { return _mm_max_ps(a.data, b.data); }

	[[nodiscard]] float MinComponent() const
	{
		__m128 s1	= _mm_shuffle_ps(data, data, _MM_SHUFFLE(0, 0, 0, 1));
		__m128 mn01 = _mm_min_ss(data, s1);
		__m128 s2	= _mm_shuffle_ps(data, data, _MM_SHUFFLE(0, 0, 0, 2));
		return _mm_cvtss_f32(_mm_min_ss(mn01, s2));
	}
	[[nodiscard]] float MaxComponent() const
	{
		__m128 s1	= _mm_shuffle_ps(data, data, _MM_SHUFFLE(0, 0, 0, 1));
		__m128 mx01 = _mm_max_ss(data, s1);
		__m128 s2	= _mm_shuffle_ps(data, data, _MM_SHUFFLE(0, 0, 0, 2));
		return _mm_cvtss_f32(_mm_max_ss(mx01, s2));
	}

	__m128 GetRaw() const { return data; }

	[[nodiscard]] Vec3 Cross(const Vec3& other) const
	{
		__m128 a = _mm_shuffle_ps(data, data, _MM_SHUFFLE(3, 0, 2, 1));
		__m128 b = _mm_shuffle_ps(other.data, other.data, _MM_SHUFFLE(3, 1, 0, 2));
		__m128 c = _mm_shuffle_ps(data, data, _MM_SHUFFLE(3, 1, 0, 2));
		__m128 d = _mm_shuffle_ps(other.data, other.data, _MM_SHUFFLE(3, 0, 2, 1));
		return _mm_sub_ps(_mm_mul_ps(a, b), _mm_mul_ps(c, d));
	}

	float Dot(const Vec3& other) const { return _mm_cvtss_f32(_mm_dp_ps(data, other.data, 0x71)); }
};
} // namespace Utils::SSE
