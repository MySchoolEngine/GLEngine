#pragma once

#include <Physics/Primitives/Intersectable.h>
#include <Physics/Primitives/Ray.h>

namespace GLEngine::Physics::Primitives {

struct S_Triangle : public T_Intersectable<S_Triangle> {
	S_Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
		: m_p({p0, p1, p2})
	{
		const auto normal = glm::cross(m_p[1] - m_p[0], m_p[2] - m_p[0]);
		m_Area			  = glm::length(normal) / 2.f;
		m_Normal		  = glm::normalize(normal);
	}

	[[nodiscard]] inline float IntersectImpl(const S_Ray& ray) const
	{
		using namespace glm;
		const auto ao = m_p[0] - ray.origin;
		const auto bo = m_p[1] - ray.origin;
		const auto co = m_p[2] - ray.origin;

		const auto v0 = cross(co, bo);
		const auto v1 = cross(bo, ao);
		const auto v2 = cross(ao, co);

		const auto v0d = dot(v0, ray.direction);
		const auto v1d = dot(v1, ray.direction);
		const auto v2d = dot(v2, ray.direction);

		if (((v0d < 0.f) && (v1d < 0.f) && (v2d < 0.f)) || ((v0d >= 0.f) && (v1d >= 0.f) && (v2d >= 0.f)))
		{
			const float distance = dot(m_Normal, ao) / dot(m_Normal, ray.direction);

			return distance;
		}

		return -1.f;
	}

	[[nodiscard]] inline glm::vec3 GetNormal() const { return m_Normal; }
	[[nodiscard]] inline float	   GetArea() const { return m_Area; }

	std::array<glm::vec3, 3> m_p;

private:
	glm::vec3 m_Normal;
	float	  m_Area;
};
} // namespace GLEngine::Physics::Primitives