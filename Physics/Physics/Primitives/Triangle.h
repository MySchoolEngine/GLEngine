#pragma once

#include <Physics/GeometryUtils/TriangleIntersect.h>
#include <Physics/Primitives/Intersectable.h>
#include <Physics/Primitives/Ray.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Physics::Primitives {

struct S_Triangle final : public T_Intersectable<S_Triangle> {
	S_Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
		: m_p({p0, p1, p2})
	{
		const auto normal = glm::cross(m_p[1] - m_p[0], m_p[2] - m_p[0]);
		const auto length = glm::length(normal);
		m_Area			  = length / 2.f;

		// Check for degenerate triangle (collinear points)
		constexpr float EPSILON = 1e-8f;
		if (length > EPSILON)
		{
			m_Normal = glm::normalize(normal);
		}
		else
		{
			// Degenerate triangle - use default normal
			m_Normal = glm::vec3(0.0f, 0.0f, 1.0f);
			GLE_ASSERT(false, "Triangle constructor called with collinear points, resulting in degenerate triangle");
		}
	}

	[[nodiscard]] inline float IntersectImpl(const S_Ray& ray) const
	{
		glm::vec2  barycentric;
		const auto distance = TriangleRayIntersect(m_p, ray, &barycentric);
		if (distance < 0)
			return distance;


		return distance;
	}

	[[nodiscard]] glm::vec3 GetNormal() const { return m_Normal; }
	[[nodiscard]] float		GetArea() const { return m_Area; }

	std::array<glm::vec3, 3> m_p;

	RTTR_REGISTRATION_FRIEND

private:
	glm::vec3 m_Normal;
	float	  m_Area;
};
} // namespace GLEngine::Physics::Primitives