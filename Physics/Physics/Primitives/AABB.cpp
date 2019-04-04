#include <Physics/Primitives/AABB.h>

#include <Physics/Primitives/Shapes.h>

#include <numeric>

namespace GLEngine {
namespace Physics {
namespace Primitives {
//=================================================================================
S_AABB::S_AABB()
{
	constexpr float maxFloat = std::numeric_limits<float>::infinity();
	constexpr float minFloat = -std::numeric_limits<float>::infinity();

	m_Min = glm::vec3(maxFloat, maxFloat, maxFloat);
	m_Max = glm::vec3(minFloat, minFloat, minFloat);
}

//=================================================================================
void S_AABB::Add(const glm::vec3& point)
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

//=================================================================================
void S_AABB::Add(const S_AABB& bbox)
{
	Add(bbox.m_Max);
	Add(bbox.m_Min);
}

//=================================================================================
void S_AABB::Add(const glm::vec4& point)
{
	Add(glm::vec3(point));
}

//=================================================================================
void S_AABB::Add(const S_Sphere& sphere)
{
	throw "Needs implementation";
}

//=================================================================================
void S_AABB::updateWithTriangle(const glm::vec4* triangleVertices)
{
	Add(triangleVertices[0]);
	Add(triangleVertices[1]);
	Add(triangleVertices[2]);
}

//=================================================================================
S_Sphere S_AABB::GetSphere() const
{
	glm::vec3 center = m_Min + (m_Max - m_Min) / 2.0f;
	float radius = glm::abs(glm::length(m_Max - center));
	return S_Sphere(center, radius);
}

//=================================================================================
S_AABB S_AABB::getTransformedAABB(const glm::mat4 matrix) const
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

}
}
}
