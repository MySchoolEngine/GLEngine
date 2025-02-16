#pragma once

#include <Physics/Primitives/Intersectable.h>
#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Sphere.h>

#include <algorithm>

namespace GLEngine::Physics::Primitives {

struct S_AABB : public T_Intersectable<S_AABB> {
public:
	constexpr S_AABB()
		: m_Min(0.0f, 0.0f, 0.0f)
		, m_Max(0.0f, 0.0f, 0.0f)
		, m_Initialised(false)
	{
	}

	[[nodiscard]] inline float IntersectImpl(const S_Ray& ray) const
	{
		if (!m_Initialised)
		{
			return -1.0f;
		}

		// https://github.com/erich666/GraphicsGems/blob/master/gems/RayBox.c
		enum class E_QuadrantName : std::uint8_t{
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
			if (quadrant[i] != E_QuadrantName::MIDDLE && ray.direction[i] != 0.)
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

	constexpr void Add(const glm::vec3& point)
	{
		if (!m_Initialised)
		{
			m_Min = m_Max = point;
			m_Initialised = true;
			return;
		}

		m_Min.x = std::min(point.x, m_Min.x);
		m_Min.y = std::min(point.y, m_Min.y);
		m_Min.z = std::min(point.z, m_Min.z);

		m_Max.x = std::max(point.x, m_Max.x);
		m_Max.y = std::max(point.y, m_Max.y);
		m_Max.z = std::max(point.z, m_Max.z);
	}
	constexpr void Add(const glm::vec4& point) { Add(glm::vec3(point)); }
	constexpr void Add(const S_AABB& bbox)
	{
		if (!bbox.m_Initialised)
		{
			return;
		}
		Add(bbox.m_Max);
		Add(bbox.m_Min);
	}
	constexpr void Add(const S_Sphere& sphere)
	{
		const auto pos = sphere.m_position;
		Add(glm::vec3(pos.x + 1, pos.y, pos.z));
		Add(glm::vec3(pos.x - 1, pos.y, pos.z));
		Add(glm::vec3(pos.x, pos.y + 1, pos.z));
		Add(glm::vec3(pos.x, pos.y - 1, pos.z));
		Add(glm::vec3(pos.x, pos.y, pos.z + 1));
		Add(glm::vec3(pos.x, pos.y, pos.z - 1));
	}

	constexpr void updateWithTriangle(const glm::vec3* triangleVertices)
	{
		Add(triangleVertices[0]);
		Add(triangleVertices[1]);
		Add(triangleVertices[2]);
	}

	[[nodiscard]] S_Sphere GetSphere() const
	{
		glm::vec3 center = m_Min + (m_Max - m_Min) / 2.0f;
		float	  radius = glm::abs(glm::length(m_Max - center));
		return {center, radius};
	}
	[[nodiscard]] constexpr S_AABB getTransformedAABB(const glm::mat4& matrix) const
	{
		if (!m_Initialised)
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

	glm::vec3 m_Min;
	glm::vec3 m_Max;
	bool	  m_Initialised;
};
} // namespace GLEngine::Physics::Primitives