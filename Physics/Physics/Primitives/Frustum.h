/** ==============================================
 * @file 		Frustum.h
 * @date 		2018/03/17 19:52
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include <Core/CoreMacros.h>

#include <Physics/Primitives/AABB.h>

#include <glm/glm.hpp>



namespace GLEnglien {
namespace Physics {
namespace Primitives {
/** ==============================================
 * @class C_Frustum
 *
 * @brief Helper class for Frustum debugging and getting the AABB's
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2018/03/17
 ** ==============================================*/
class API_EXPORT C_Frustum {
public:
	C_Frustum(const glm::vec3& position, const glm::vec3& upVectro, const glm::vec3& foreward, float near, float far, float aspect, float fov);
	/** ==============================================
	 * @method:    GetAABB
	 * @fullName:  C_Frustum::GetAABB
	 * @return:    AABB
	 * @brief	   Returns AABB of whole frustum. If you want
	 *			   AABB of subfrustum, simply change far/near plane.
	 ** ==============================================*/
	S_AABB GetAABB() const;
	void DebugDraw(const glm::vec3& color) const;
	void UpdateWithMatrix(const glm::mat4& matrix);

	const glm::vec3& GetForeward() const { return m_foreward; }
	void			SetForeward(const glm::vec3& val) { m_foreward = val; }
	float			GetNear() const { return m_near; }
	void			SetNear(float val) { m_near = val; }
	float			GetFar() const { return m_far; }
	void			SetFar(float val) { m_far = val; }
	float			GetAspect() const { return m_aspect; }
	void			SetAspect(float val) { m_aspect = val; }
	float			GetFov() const { return m_fov; }
	void			SetFov(float val) { m_fov = val; }
private:
	glm::vec3	m_position;
	glm::vec3	m_upVector;
	glm::vec3	m_foreward;
	float		m_near;
	float		m_far;
	float		m_aspect;
	float		m_fov;
};
}
}
}