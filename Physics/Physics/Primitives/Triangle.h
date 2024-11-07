#pragma once

#include <Physics/GeometryUtils/TriangleIntersect.h>
#include <Physics/Primitives/Intersectable.h>
#include <Physics/Primitives/Ray.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Physics::Primitives {

struct S_Triangle : public T_Intersectable<S_Triangle> {
	S_Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
		: m_p({p0, p1, p2})
	{
		const auto normal = glm::cross(m_p[1] - m_p[0], m_p[2] - m_p[0]);
		m_Area			  = glm::length(normal) / 2.f;
		m_Normal		  = glm::normalize(normal);
	}

	[[nodiscard]] inline float IntersectImpl(const S_Ray& ray) const { return TraingleRayIntersect(m_p, ray); }

	[[nodiscard]] inline glm::vec3 GetNormal() const { return m_Normal; }
	[[nodiscard]] inline float	   GetArea() const { return m_Area; }

	std::array<glm::vec3, 3> m_p;

	RTTR_ENABLE();
	RTTR_REGISTRATION_FRIEND;

private:
	glm::vec3 m_Normal;
	float	  m_Area;
};
} // namespace GLEngine::Physics::Primitives