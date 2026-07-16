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

#pragma region--Component access
	[[nodiscard]] float x() const { return _mm_cvtss_f32(data); }
	[[nodiscard]] float y() const { return _mm_cvtss_f32(_mm_shuffle_ps(data, data, _MM_SHUFFLE(0, 0, 0, 1))); }
	[[nodiscard]] float z() const { return _mm_cvtss_f32(_mm_shuffle_ps(data, data, _MM_SHUFFLE(0, 0, 0, 2))); }

	[[nodiscard]] float operator[](int i) const
	{
		alignas(16) float v[4];
		_mm_store_ps(v, data);
		return v[i];
	}

	void SetX(float val) { data = _mm_insert_ps(data, _mm_set_ss(val), 0x00); }
	void SetY(float val) { data = _mm_insert_ps(data, _mm_set_ss(val), 0x10); }
	void SetZ(float val) { data = _mm_insert_ps(data, _mm_set_ss(val), 0x20); }
#pragma endregion-- Component access

#pragma region--Arithmetic operators
	[[nodiscard]] Vec3 operator-(const Vec3& other) const { return _mm_sub_ps(data, other.data); }
	[[nodiscard]] Vec3 operator+(const Vec3& other) const { return _mm_add_ps(data, other.data); }
	[[nodiscard]] Vec3 operator*(const Vec3& other) const { return _mm_mul_ps(data, other.data); }
	[[nodiscard]] Vec3 operator/(const Vec3& other) const { return _mm_div_ps(data, other.data); }
	[[nodiscard]] Vec3 operator/(const float val) const { return _mm_div_ps(data, _mm_set1_ps(val)); }
	[[nodiscard]] Vec3 operator*(const float val) const { return _mm_mul_ps(data, _mm_set1_ps(val)); }
#pragma endregion-- Arithmetic operators

	[[nodiscard]] bool operator!=(const Vec3& other) const { return !(*this == other); }
	[[nodiscard]] bool operator==(const Vec3& other) const
	{
		__m128 cmp	= _mm_cmpeq_ps(data, other.data); // 0xFFFFFFFF per lane if equal, 0 if not
		int	   mask = _mm_movemask_ps(cmp);			  // bit i = sign bit of lane i
		return (mask & 0x7) == 0x7;
	}

#pragma region--Conversion operators
	[[nodiscard]] explicit operator glm::vec3() const
	{
		alignas(16) float result[4];
		_mm_store_ps(result, data);
		return glm::vec3(result[0], result[1], result[2]);
	}
#pragma endregion-- Conversion operators

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

	/**
	 * @brief Checks whether x, y and z each lie within [min, max], bounds inclusive.
	 *
	 * @param min Lower bound per component.
	 * @param max Upper bound per component.
	 * @return true if x, y and z are all within their respective [min, max] range.
	 */
	[[nodiscard]] bool IsInRange(const Vec3& min, const Vec3& max) const
	{
		__m128 ltMin = _mm_cmplt_ps(data, min.GetRaw()); // p < min
		__m128 gtMax = _mm_cmpgt_ps(data, max.GetRaw()); // p > max
		__m128 out	 = _mm_or_ps(ltMin, gtMax);
		return (_mm_movemask_ps(out) & 0x7) == 0;
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

[[nodiscard]] inline float distance2(const Vec3& a, const Vec3& b)
{
	const Vec3 diff = a - b;
	return diff.Dot(diff);
}

#pragma region--Arithmetic operators
[[nodiscard]] inline Vec3 operator*(const float x, const Vec3& y)
{
	return y * x;
}
#pragma endregion-- Arithmetic operators
} // namespace Utils::SSE
