#include "Shapes.h"

//=================================================================================
Shapes::S_Sphere::S_Sphere(const glm::vec3& position, float radius)
	: m_position(position)
	, m_radius(radius)
{

}

//=================================================================================
bool Shapes::S_Sphere::IsColliding(const S_Sphere& other) const
{
	auto distance = glm::abs(glm::length(m_position - other.m_position));
	if (distance < m_radius + other.m_radius) {
		return true;
	}
	return false;
}

//=================================================================================
void Shapes::S_Sphere::Transform(const glm::mat4& matrix)
{
	m_position = glm::vec3(matrix * glm::vec4(m_position, 1.0f));
}
