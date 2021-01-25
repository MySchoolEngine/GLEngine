/** ==============================================
 * @file 		Shapes.h
 * @date 		2018/05/16 21:32
 * @project 	Opengl Shadow Rendering
 * @faculty 	Faculty of Information Technology
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/

#pragma once

#include <glm/glm.hpp>

namespace GLEngine::Physics::Primitives {
/** ==============================================
 * @class S_Sphere
 *
 * @brief	Represents 3D sphere
 *
 * @author 	Dominik Rohacek
 * Contact:	RohacekD@gmail.com
 * @date 	2018/05/16
 ** ==============================================*/
struct S_Sphere {
public:
	S_Sphere() = default;
	constexpr S_Sphere(const glm::vec3& position, float radius)
		: m_position(position)
		, m_radius(radius)
	{
	}

	[[nodiscard]] bool IsColliding(const S_Sphere& other) const
	{
		auto distance = glm::abs(glm::length(m_position - other.m_position));
		if (distance < m_radius + other.m_radius)
		{
			return true;
		}
		return false;
	}
	void Transform(const glm::mat4& matrix) { m_position = glm::vec3(matrix * glm::vec4(m_position, 1.0f)); }

	glm::vec3 m_position;
	float	  m_radius;
};
} // namespace GLEngine::Physics::Primitives