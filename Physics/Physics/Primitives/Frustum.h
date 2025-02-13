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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

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
	constexpr C_Frustum(const glm::vec3& position, const glm::vec3& upVectror, const glm::vec3& forward, float znear, float zfar, float aspect, float fov)
		: m_Position(position)
		, m_UpVector(upVectror)
		, m_Forward(forward)
		, m_Near(znear)
		, m_Far(zfar)
		, m_Aspect(aspect)
		, m_Fov(fov)
	{
	}
	/** ==============================================
	 * @method:    GetAABB
	 * @fullName:  C_Frustum::GetAABB
	 * @return:    AABB
	 * @brief	   Returns AABB of whole frustum. If you want
	 *			   AABB of subfrustum, simply change far/near plane.
	 ** ==============================================*/
	[[nodiscard]] S_AABB GetAABB() const
	{
		glm::vec3 nearCenter = glm::vec3(m_Position + (m_Forward * m_Near));
		glm::vec3 farCenter	 = glm::vec3(m_Position + (m_Forward * m_Far));

		glm::vec3 left = glm::cross(m_UpVector, m_Forward);

		float tanHalfHFOV = tanf(glm::radians(m_Fov / 2.0f));
		float tanHalfVFOV = tanf(glm::radians((m_Fov * m_Aspect) / 2.0f));

		float xn = m_Near * tanHalfVFOV;
		float xf = m_Far * tanHalfVFOV;
		float yn = m_Near * tanHalfHFOV;
		float yf = m_Far * tanHalfHFOV;

		glm::vec4 nlt = glm::vec4(nearCenter + (xn * left) + m_UpVector * yn, 1.0f);
		glm::vec4 nrt = glm::vec4(nearCenter - (xn * left) + m_UpVector * yn, 1.0f);
		glm::vec4 nlb = glm::vec4(nearCenter + (xn * left) - m_UpVector * yn, 1.0f);
		glm::vec4 nrb = glm::vec4(nearCenter - (xn * left) - m_UpVector * yn, 1.0f);
		glm::vec4 flt = glm::vec4(farCenter + (xf * left) + m_UpVector * yf, 1.0f);
		glm::vec4 frt = glm::vec4(farCenter - (xf * left) + m_UpVector * yf, 1.0f);
		glm::vec4 flb = glm::vec4(farCenter + (xf * left) - m_UpVector * yf, 1.0f);
		glm::vec4 frb = glm::vec4(farCenter - (xf * left) - m_UpVector * yf, 1.0f);

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
		auto newPosition = glm::vec3((matrix * glm::vec4(m_Position, 1.0f)));
		m_UpVector		 = glm::vec3((matrix * glm::vec4(m_Position + m_UpVector, 1.0f))) - newPosition;
		m_Forward		 = glm::vec3((matrix * glm::vec4(m_Position + m_Forward, 1.0f))) - newPosition;

		m_Position = newPosition;
	}

	[[nodiscard]] glm::mat4 GetViewMatrix() const { return glm::lookAt(GetPosition(), GetPosition() + GetForward(), GetUpVector()); }

	[[nodiscard]] constexpr const glm::vec3& GetForward() const { return m_Forward; }
	constexpr void							 SetForward(const glm::vec3& val) { m_Forward = val; }
	[[nodiscard]] constexpr float			 GetNear() const { return m_Near; }
	constexpr void							 SetNear(float val) { m_Near = val; }
	[[nodiscard]] constexpr float			 GetFar() const { return m_Far; }
	constexpr void							 SetFar(float val) { m_Far = val; }
	[[nodiscard]] constexpr float			 GetAspect() const { return m_Aspect; }
	constexpr void							 SetAspect(float val) { m_Aspect = val; }
	[[nodiscard]] constexpr float			 GetFov() const { return m_Fov; }
	constexpr void							 SetFov(float val) { m_Fov = val; }
	[[nodiscard]] constexpr const glm::vec3& GetPosition() const { return m_Position; }
	[[nodiscard]] constexpr const glm::vec3& GetUpVector() const { return m_UpVector; }

private:
	glm::vec3 m_Position;
	glm::vec3 m_UpVector;
	glm::vec3 m_Forward;
	float	  m_Near;
	float	  m_Far;
	float	  m_Aspect;
	float	  m_Fov;
};
} // namespace GLEngine::Physics::Primitives