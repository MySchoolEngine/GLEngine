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
		// Möller–Trumbore intersection algorithm
		const float EPSILON = 0.0000001f;
		using namespace glm;
		vec3  edge1, edge2, h, s, q;
		float a, f, u, v;
		edge1 = m_p[1] - m_p[0];
		edge2 = m_p[2] - m_p[0];
		h	  = cross(ray.direction, edge2);
		a	  = dot(edge1, h);
		if (a > -EPSILON && a < EPSILON)
			return false; // This ray is parallel to this triangle.
		f = 1.0f / a;
		s = ray.origin - m_p[0];
		u = f * dot(s, h);
		if (u < 0.0f || u > 1.0f)
			return false;
		q = cross(s, edge1);
		v = f * dot(ray.direction, q);
		if (v < 0.0f || u + v > 1.0f)
			return false;
		// At this stage we can compute t to find out where the intersection point is on the line.
		float t = f * dot(edge2, q);
		if (t > EPSILON) // ray intersection
		{
			return t;
		}
		else // This means that there is a line intersection but not a ray intersection.
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