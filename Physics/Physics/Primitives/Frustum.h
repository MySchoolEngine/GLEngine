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

#include <Physics/Primitives/AABB.h>

#include <glm/glm.hpp>

namespace GLEngine::Physics::Primitives {

/** ==============================================
 * @class C_Frustum
 *
 * @brief Helper class for Frustum debugging and getting the AABB's
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2018/03/17
 ** ==============================================*/
class C_Frustum {
public:
	constexpr C_Frustum(const glm::vec3& position, const glm::vec3& upVectro, const glm::vec3& foreward, float znear, float zfar, float aspect, float fov)
		: m_position(position)
		, m_upVector(upVectro)
		, m_foreward(foreward)
		, m_near(znear)
		, m_far(zfar)
		, m_aspect(aspect)
		, m_fov(fov)
	{}
	/** ==============================================
	 * @method:    GetAABB
	 * @fullName:  C_Frustum::GetAABB
	 * @return:    AABB
	 * @brief	   Returns AABB of whole frustum. If you want
	 *			   AABB of subfrustum, simply change far/near plane.
	 ** ==============================================*/
	[[nodiscard]] S_AABB GetAABB() const
	{
		glm::vec3 nearCenter = glm::vec3(m_position + (m_foreward * m_near));
		glm::vec3 farCenter = glm::vec3(m_position + (m_foreward * m_far));

		glm::vec3 left = glm::cross(m_upVector, m_foreward);

		float tanHalfHFOV = tanf(glm::radians(m_fov / 2.0f));
		float tanHalfVFOV = tanf(glm::radians((m_fov * m_aspect) / 2.0f));

		float xn = m_near * tanHalfVFOV;
		float xf = m_far * tanHalfVFOV;
		float yn = m_near * tanHalfHFOV;
		float yf = m_far * tanHalfHFOV;

		glm::vec4 nlt = glm::vec4(nearCenter + (xn * left) + m_upVector * yn, 1.0f);
		glm::vec4 nrt = glm::vec4(nearCenter - (xn * left) + m_upVector * yn, 1.0f);
		glm::vec4 nlb = glm::vec4(nearCenter + (xn * left) - m_upVector * yn, 1.0f);
		glm::vec4 nrb = glm::vec4(nearCenter - (xn * left) - m_upVector * yn, 1.0f);
		glm::vec4 flt = glm::vec4(farCenter + (xf * left) + m_upVector * yf, 1.0f);
		glm::vec4 frt = glm::vec4(farCenter - (xf * left) + m_upVector * yf, 1.0f);
		glm::vec4 flb = glm::vec4(farCenter + (xf * left) - m_upVector * yf, 1.0f);
		glm::vec4 frb = glm::vec4(farCenter - (xf * left) - m_upVector * yf, 1.0f);

		S_AABB bbox;
		bbox.Add(nlt);
		bbox.Add(nrt);
		bbox.Add(nlb);
		bbox.Add(nrb);
		bbox.Add(flt);
		bbox.Add(frt);
		bbox.Add(flb);
		bbox.Add(frb);
		return bbox;
	}
	void UpdateWithMatrix(const glm::mat4& matrix)
	{
		auto newPosition = glm::vec3((matrix * glm::vec4(m_position, 1.0f)));
		m_upVector = glm::vec3((matrix * glm::vec4(m_position + m_upVector, 1.0f))) - newPosition;
		m_foreward = glm::vec3((matrix * glm::vec4(m_position + m_foreward, 1.0f))) - newPosition;

		m_position = newPosition;
	}

	[[nodiscard]] constexpr const glm::vec3& GetForeward() const { return m_foreward; }
	constexpr void			SetForeward(const glm::vec3& val) { m_foreward = val; }
	[[nodiscard]] constexpr float			GetNear() const { return m_near; }
	constexpr void			SetNear(float val) { m_near = val; }
	[[nodiscard]] constexpr float			GetFar() const { return m_far; }
	constexpr void			SetFar(float val) { m_far = val; }
	[[nodiscard]] constexpr float			GetAspect() const { return m_aspect; }
	constexpr void			SetAspect(float val) { m_aspect = val; }
	[[nodiscard]] constexpr float			GetFov() const { return m_fov; }
	constexpr void			SetFov(float val) { m_fov = val; }
	[[nodiscard]] constexpr const glm::vec3& GetPosition() const { return m_position; }
	[[nodiscard]] constexpr const glm::vec3& GetUpVector() const { return m_upVector; }
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