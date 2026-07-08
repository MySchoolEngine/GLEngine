#pragma once

#include <Physics/Primitives/Intersectable.h>
#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Sphere.h>

#include <algorithm>
#include <xmmintrin.h>
#define VEC3TOSSE(vec) 0.f, vec.z, vec.y, vec.x

namespace GLEngine::Physics::Primitives {

struct S_AABB : public T_Intersectable<S_AABB> {
public:
	constexpr S_AABB()
		: m_Min(std::numeric_limits<float>::infinity())
		, m_Max(-std::numeric_limits<float>::infinity())
	{
	}

	[[nodiscard]] float Intersects(const S_SSERay& ray) const noexcept
	{
		using namespace ::Utils::SSE;
		if (!IsInitialized())
		{
			return std::numeric_limits<float>::infinity();
		}
		const Vec3 minm(m_Min);
		const Vec3 maxm(m_Max);
		Vec3	   t1 = (minm - ray.origin) * ray.invDirection;
		Vec3	   t2 = (maxm - ray.origin) * ray.invDirection;
		// 0 * ±inf = NaN when origin lies exactly on an AABB boundary and ray is parallel to that axis.
		// Replace NaN in t1m with -inf (slab entry: no constraint) and in t2m with +inf (slab exit: no constraint).
		__m128 nanMask = _mm_cmpunord_ps(t1.GetRaw(), t1.GetRaw());
		t1			   = _mm_or_ps(_mm_andnot_ps(nanMask, t1.GetRaw()), _mm_and_ps(nanMask, _mm_set1_ps(-std::numeric_limits<float>::infinity())));
		nanMask		   = _mm_cmpunord_ps(t2.GetRaw(), t2.GetRaw());
		t2			   = _mm_or_ps(_mm_andnot_ps(nanMask, t2.GetRaw()), _mm_and_ps(nanMask, _mm_set1_ps(std::numeric_limits<float>::infinity())));
		Vec3 tenter	   = Vec3::min(t1, t2);
		Vec3 texit	   = Vec3::max(t1, t2);

		float		tmin = tenter.MaxComponent();
		const float tmax = texit.MinComponent();

		tmin = std::max(tmin, 0.f);
		return (tmin <= tmax) ? tmin : std::numeric_limits<float>::infinity();
	}

	[[nodiscard]] float Intersects(const S_Ray& ray) const noexcept
	{
		using namespace ::Utils::SSE;
		if (!IsInitialized())
		{
			return std::numeric_limits<float>::infinity();
		}
		const Vec3 minm(m_Min);
		const Vec3 maxm(m_Max);
		const Vec3 originm(ray.origin);
		Vec3	   t1 = (minm - originm) * ray.invDirection;
		Vec3	   t2 = (maxm - originm) * ray.invDirection;
		// 0 * ±inf = NaN when origin lies exactly on an AABB boundary and ray is parallel to that axis.
		// Replace NaN in t1m with -inf (slab entry: no constraint) and in t2m with +inf (slab exit: no constraint).
		__m128 nanMask = _mm_cmpunord_ps(t1.GetRaw(), t1.GetRaw());
		t1			   = _mm_or_ps(_mm_andnot_ps(nanMask, t1.GetRaw()), _mm_and_ps(nanMask, _mm_set1_ps(-std::numeric_limits<float>::infinity())));
		nanMask		   = _mm_cmpunord_ps(t2.GetRaw(), t2.GetRaw());
		t2			   = _mm_or_ps(_mm_andnot_ps(nanMask, t2.GetRaw()), _mm_and_ps(nanMask, _mm_set1_ps(std::numeric_limits<float>::infinity())));
		Vec3 tenter	   = Vec3::min(t1, t2);
		Vec3 texit	   = Vec3::max(t1, t2);

		float		tmin = tenter.MaxComponent();
		const float tmax = texit.MinComponent();

		tmin = std::max(tmin, 0.f);
		return (tmin <= tmax) ? tmin : std::numeric_limits<float>::infinity();
		// float tmin = 0.0, tmax = INFINITY;
		//
		// const glm::vec3 t1 = (m_Min - ray.origin) * ray.invDirection;
		// const glm::vec3 t2 = (m_Max - ray.origin) * ray.invDirection;
		// for (int d = 0; d < 3; ++d)
		// {
		//
		// 	tmin = std::min(std::max(t1[d], tmin), std::max(t2[d], tmin));
		// 	tmax = std::max(std::min(t1[d], tmax), std::min(t2[d], tmax));
		// }
		//
		// return (tmin <= tmax) ? tmin : std::numeric_limits<float>::infinity();
	}

	[[nodiscard]] inline float IntersectImpl(const S_Ray& ray) const
	{
		if (!IsInitialized())
		{
			return -1.0f;
		}

		// https://github.com/erich666/GraphicsGems/blob/master/gems/RayBox.c
		enum class E_QuadrantName : std::uint8_t {
			RIGHT,
			LEFT,
			MIDDLE
		};
		bool				 inside = true;
		std::array<float, 3> candidatePlane;
		std::array<float, 3> maxT;
		E_QuadrantName		 quadrant[3];
		for (int i = 0; i < 3; ++i)
		{
			if (ray.origin[i] < m_Min[i])
			{
				quadrant[i]		  = E_QuadrantName::LEFT;
				candidatePlane[i] = m_Min[i];
				inside			  = false;
			}
			else if (ray.origin[i] > m_Max[i])
			{
				quadrant[i]		  = E_QuadrantName::RIGHT;
				candidatePlane[i] = m_Max[i];
				inside			  = false;
			}
			else
			{
				quadrant[i] = E_QuadrantName::MIDDLE;
			}
		}

		if (inside)
		{
			return 0.0f;
		}

		for (int i = 0; i < 3; i++)
			if (quadrant[i] != E_QuadrantName::MIDDLE && std::abs(ray.direction[i]) >= s_RayDirectionEpsilon)
				maxT[i] = (candidatePlane[i] - ray.origin[i]) / ray.direction[i];
			else
				maxT[i] = -1.;

		int whichPlane = 0;
		for (int i = 1; i < 3; i++)
			if (maxT[whichPlane] < maxT[i])
				whichPlane = i;

		/* Check final candidate actually inside box */
		if (maxT[whichPlane] < 0)
			return maxT[whichPlane];

		glm::vec3 coord;

		for (int i = 0; i < 3; i++)
			if (whichPlane != i)
			{
				coord[i] = ray.origin[i] + maxT[whichPlane] * ray.direction[i];
				if (coord[i] < m_Min[i] || coord[i] > m_Max[i])
					return -1.f;
			}
			else
			{
				coord[i] = candidatePlane[i];
			}

		return glm::distance(ray.origin, coord);
	}

	void Add(const glm::vec3& point)
	{
		using namespace ::Utils::SSE;
		Vec3 pointm(point);
		Vec3 minm(m_Min);
		Vec3 maxm(m_Max);
		minm  = Vec3::min(minm, pointm);
		maxm  = Vec3::max(maxm, pointm);
		m_Min = static_cast<glm::vec3>(minm);
		m_Max = static_cast<glm::vec3>(maxm);
	}
	void Add(const glm::vec4& point) { Add(glm::vec3(point)); }
	void Add(const S_AABB& bbox)
	{
		if (!bbox.IsInitialized())
		{
			return;
		}
		Add(bbox.m_Max);
		Add(bbox.m_Min);
	}
	void Add(const S_Sphere& sphere)
	{
		const auto pos = sphere.m_position;
		Add(glm::vec3(pos.x + sphere.m_radius, pos.y, pos.z));
		Add(glm::vec3(pos.x - sphere.m_radius, pos.y, pos.z));
		Add(glm::vec3(pos.x, pos.y + sphere.m_radius, pos.z));
		Add(glm::vec3(pos.x, pos.y - sphere.m_radius, pos.z));
		Add(glm::vec3(pos.x, pos.y, pos.z + sphere.m_radius));
		Add(glm::vec3(pos.x, pos.y, pos.z - sphere.m_radius));
	}

	void updateWithTriangle(const glm::vec3* triangleVertices)
	{
		__m128 minm = _mm_set_ps(VEC3TOSSE(m_Min));
		__m128 maxm = _mm_set_ps(VEC3TOSSE(m_Max));
		for (int i = 0; i < 3; ++i)
		{
			__m128 pointm = _mm_set_ps(VEC3TOSSE(triangleVertices[i]));

			minm = _mm_min_ps(minm, pointm);
			maxm = _mm_max_ps(maxm, pointm);
		}
		alignas(16) float min[4];
		alignas(16) float max[4];
		_mm_store_ps(min, minm);
		_mm_store_ps(max, maxm);
		memcpy(&m_Min, min, sizeof(float) * 3);
		memcpy(&m_Max, max, sizeof(float) * 3);
	}

	[[nodiscard]] S_Sphere GetSphere() const
	{
		glm::vec3 center = m_Min + (m_Max - m_Min) / 2.0f;
		float	  radius = glm::abs(glm::length(m_Max - center));
		return {center, radius};
	}
	[[nodiscard]] constexpr S_AABB getTransformedAABB(const glm::mat4& matrix) const
	{
		if (!IsInitialized())
		{
			return {};
		}
		S_AABB newBB;

		glm::vec3 size = m_Max - m_Min;

		newBB.Add(glm::vec3(matrix * glm::vec4(m_Min + glm::vec3(size.x, 0.0f, 0.0f), 1.0f)));
		newBB.Add(glm::vec3(matrix * glm::vec4(m_Min + glm::vec3(size.x, size.y, 0.0f), 1.0f)));
		newBB.Add(glm::vec3(matrix * glm::vec4(m_Min + glm::vec3(size.x, 0.0f, size.z), 1.0f)));
		newBB.Add(glm::vec3(matrix * glm::vec4(m_Min + glm::vec3(0.0f, size.y, 0.0f), 1.0f)));
		newBB.Add(glm::vec3(matrix * glm::vec4(m_Min + glm::vec3(0.0f, 0.0f, size.z), 1.0f)));
		newBB.Add(glm::vec3(matrix * glm::vec4(m_Min + glm::vec3(0.0f, size.y, size.z), 1.0f)));
		newBB.Add(glm::vec3(matrix * glm::vec4(m_Min, 1.0f)));
		newBB.Add(glm::vec3(matrix * glm::vec4(m_Max, 1.0f)));

		return newBB;
	}

	constexpr float Area() const
	{
		const glm::vec3 extent = m_Max - m_Min;
		return (extent.x * extent.y + extent.y * extent.z + extent.z * extent.x) * 2.f;
	}

	[[nodiscard]] constexpr bool Contains(const glm::vec3& point) const
	{
		return point.x >= m_Min.x && point.x <= m_Max.x && point.y >= m_Min.y && point.y <= m_Max.y && point.z >= m_Min.z && point.z <= m_Max.z;
	}

	[[nodiscard]] constexpr bool Contains(const S_AABB& other) const
	{
		if (!IsInitialized() || !other.IsInitialized())
		{
			return false;
		}
		return Contains(other.m_Min) && Contains(other.m_Max);
	}

	[[nodiscard]] constexpr bool IsInitialized() const
	{
		// std::isinf is constexpr only after C++23
		return m_Min.x != std::numeric_limits<float>::infinity();
	}

	glm::vec3 m_Min;
	glm::vec3 m_Max;

private:
	static constexpr float s_RayDirectionEpsilon = 1e-8f;
};

struct S_SSEAABB {
public:
	S_SSEAABB()
		: m_Min(std::numeric_limits<float>::infinity())
		, m_Max(-std::numeric_limits<float>::infinity())
	{
	}

	[[nodiscard]] float Intersects(const S_SSERay& ray) const noexcept
	{
		using namespace ::Utils::SSE;
		if (!IsInitialized())
		{
			return std::numeric_limits<float>::infinity();
		}
		Vec3 t1 = (m_Min - ray.origin) * ray.invDirection;
		Vec3 t2 = (m_Max - ray.origin) * ray.invDirection;
		// 0 * ±inf = NaN when origin lies exactly on an AABB boundary and ray is parallel to that axis.
		// Replace NaN in t1 with -inf (slab entry: no constraint) and in t2 with +inf (slab exit: no constraint).
		__m128 nanMask = _mm_cmpunord_ps(t1.GetRaw(), t1.GetRaw());
		t1			   = _mm_or_ps(_mm_andnot_ps(nanMask, t1.GetRaw()), _mm_and_ps(nanMask, _mm_set1_ps(-std::numeric_limits<float>::infinity())));
		nanMask		   = _mm_cmpunord_ps(t2.GetRaw(), t2.GetRaw());
		t2			   = _mm_or_ps(_mm_andnot_ps(nanMask, t2.GetRaw()), _mm_and_ps(nanMask, _mm_set1_ps(std::numeric_limits<float>::infinity())));
		Vec3 tenter	   = Vec3::min(t1, t2);
		Vec3 texit	   = Vec3::max(t1, t2);

		float		tmin = tenter.MaxComponent();
		const float tmax = texit.MinComponent();

		tmin = std::max(tmin, 0.f);
		return (tmin <= tmax) ? tmin : std::numeric_limits<float>::infinity();
	}

	void Add(const ::Utils::SSE::Vec3& point)
	{
		m_Min = ::Utils::SSE::Vec3::min(m_Min, point);
		m_Max = ::Utils::SSE::Vec3::max(m_Max, point);
	}
	void Add(const S_SSEAABB& bbox)
	{
		if (!bbox.IsInitialized())
		{
			return;
		}
		Add(bbox.m_Max);
		Add(bbox.m_Min);
	}
	void updateWithTriangle(const glm::vec3* triangleVertices)
	{
		for (int i = 0; i < 3; ++i)
		{
			Add(::Utils::SSE::Vec3(triangleVertices[i]));
		}
	}

	[[nodiscard]] float Area() const
	{
		const ::Utils::SSE::Vec3 extent = m_Max - m_Min;
		// rotate components: (x,y,z,0) → (y,z,x,0)
		const __m128 shuf = _mm_shuffle_ps(extent.GetRaw(), extent.GetRaw(), _MM_SHUFFLE(3, 0, 2, 1));
		// pairwise multiply: (xy, yz, zx, 0)
		__m128 prod = _mm_mul_ps(extent.GetRaw(), shuf);
		// horizontal sum of 3 lanes into lane 0
		prod = _mm_add_ss(prod, _mm_shuffle_ps(prod, prod, _MM_SHUFFLE(1, 1, 1, 1))); // xy+yz
		prod = _mm_add_ss(prod, _mm_shuffle_ps(prod, prod, _MM_SHUFFLE(2, 2, 2, 2))); // +zx
		return _mm_cvtss_f32(prod) * 2.f;
	}

	[[nodiscard]] bool Contains(const ::Utils::SSE::Vec3& point) const { return point.IsInRange(m_Min, m_Max); }

	[[nodiscard]] bool IsInitialized() const
	{
		__m128 inf = _mm_set1_ps(std::numeric_limits<float>::infinity());
		__m128 cmp = _mm_cmpeq_ps(m_Min.GetRaw(), inf);
		return (_mm_movemask_ps(cmp) & 0x7) == 0; // no xyz lane is +inf
	}

	::Utils::SSE::Vec3 m_Min;
	::Utils::SSE::Vec3 m_Max;
};
} // namespace GLEngine::Physics::Primitives