#pragma once

#include <Physics/Primitives/Shapes.h>

#include <glm/glm.hpp>

namespace GLEngine::Physics::Primitives {

struct S_AABB {
private:
	constexpr static float maxFloat = std::numeric_limits<float>::infinity();
	constexpr static float minFloat = -std::numeric_limits<float>::infinity();
public:
	constexpr S_AABB()
		: m_Min(maxFloat, maxFloat, maxFloat)
		, m_Max(minFloat, minFloat, minFloat)
	{}

	constexpr void Add(const glm::vec3& point)
	{
		if (point.x < m_Min.x)
			m_Min.x = point.x;

		if (point.y < m_Min.y)
			m_Min.y = point.y;

		if (point.z < m_Min.z)
			m_Min.z = point.z;

		if (point.x > m_Max.x)
			m_Max.x = point.x;

		if (point.y > m_Max.y)
			m_Max.y = point.y;

		if (point.z > m_Max.z)
			m_Max.z = point.z;
	}
	constexpr void Add(const glm::vec4& point)
	{
		Add(glm::vec3(point));
	}
	constexpr void Add(const S_AABB& bbox)
	{
		Add(bbox.m_Max);
		Add(bbox.m_Min);
	}
	constexpr void Add(const S_Sphere& sphere)
	{
		const auto pos = sphere.m_position;
		Add(glm::vec3(pos.x+1, pos.y, pos.z));
		Add(glm::vec3(pos.x-1, pos.y, pos.z));
		Add(glm::vec3(pos.x, pos.y+1, pos.z));
		Add(glm::vec3(pos.x, pos.y-1, pos.z));
		Add(glm::vec3(pos.x, pos.y, pos.z+1));
		Add(glm::vec3(pos.x, pos.y, pos.z-1));
	}

	constexpr void updateWithTriangle(const glm::vec4* triangleVertices)
	{
		Add(triangleVertices[0]);
		Add(triangleVertices[1]);
		Add(triangleVertices[2]);
	}

	[[nodiscard]] constexpr S_Sphere GetSphere() const
	{
		glm::vec3 center = m_Min + (m_Max - m_Min) / 2.0f;
		float radius = glm::abs(glm::length(m_Max - center));
		return S_Sphere(center, radius);
	}
	[[nodiscard]] constexpr S_AABB getTransformedAABB(const glm::mat4 matrix) const
	{
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

	glm::vec3 m_Min;
	glm::vec3 m_Max;
};
}