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

#include <Core/CoreMacros.h>

#include <glm/glm.hpp>

namespace Physics {
namespace Primitives {
/** ==============================================
* @class S_Sphere
*
* @brief	Represents 3D sphere
*
* @author 	Dominik Rohacek
* Contact:	RohacekD@gmail.com
* @date 	2018/05/16
** ==============================================*/
struct API_EXPORT S_Sphere {
public:
	S_Sphere() = default;
	S_Sphere(const glm::vec3& position, float radius);

	bool IsColliding(const S_Sphere& other) const;
	void Transform(const glm::mat4& matrix);

	glm::vec3	m_position;
	float		m_radius;
};
}
}