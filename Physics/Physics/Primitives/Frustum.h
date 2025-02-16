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
	constexpr C_Frustum(const glm::vec3& position, const glm::vec3& upVector, const glm::vec3& forward, const float zNear, const float zFar, const float aspect, const float fov)
		: m_Position(position)
		, m_UpVector(upVector)
		, m_Forward(forward)
		, m_Near(zNear)
		, m_Far(zFar)
		, m_Aspect(aspect)
		, m_Fov(fov)
	{
	}
	/** ==============================================
	 * @method:    GetAABB
	 * @fullName:  C_Frustum::GetAABB
	 * @return:    AABB
	 * @brief	   Returns AABB of whole frustum. If you want
	 *			   AABB of sub-frustum, simply change far/near plane.
	 ** ==============================================*/
	[[nodiscard]] S_AABB GetAABB() const
	{
		const auto nearCenter = glm::vec3(m_Position + (m_Forward * m_Near));
		const auto farCenter  = glm::vec3(m_Position + (m_Forward * m_Far));

		const glm::vec3 left = glm::cross(m_UpVector, m_Forward);

		const float tanHalfHFov = tanf(glm::radians(m_Fov / 2.0f));
		const float tanHalfVFov = tanf(glm::radians((m_Fov * m_Aspect) / 2.0f));

		const float xn = m_Near * tanHalfVFov;
		const float xf = m_Far * tanHalfVFov;
		const float yn = m_Near * tanHalfHFov;
		const float yf = m_Far * tanHalfHFov;

		const auto nlt = glm::vec4(nearCenter + (xn * left) + m_UpVector * yn, 1.0f);
		const auto nrt = glm::vec4(nearCenter - (xn * left) + m_UpVector * yn, 1.0f);
		const auto nlb = glm::vec4(nearCenter + (xn * left) - m_UpVector * yn, 1.0f);
		const auto nrb = glm::vec4(nearCenter - (xn * left) - m_UpVector * yn, 1.0f);
		const auto flt = glm::vec4(farCenter + (xf * left) + m_UpVector * yf, 1.0f);
		const auto frt = glm::vec4(farCenter - (xf * left) + m_UpVector * yf, 1.0f);
		const auto flb = glm::vec4(farCenter + (xf * left) - m_UpVector * yf, 1.0f);
		const auto frb = glm::vec4(farCenter - (xf * left) - m_UpVector * yf, 1.0f);

		S_AABB aabb;
		aabb.Add(nlt);
		aabb.Add(nrt);
		aabb.Add(nlb);
		aabb.Add(nrb);
		aabb.Add(flt);
		aabb.Add(frt);
		aabb.Add(flb);
		aabb.Add(frb);
		return aabb;
	}
	void UpdateWithMatrix(const glm::mat4& matrix)
	{
		const auto newPosition = glm::vec3((matrix * glm::vec4(m_Position, 1.0f)));
		m_UpVector			   = glm::vec3((matrix * glm::vec4(m_Position + m_UpVector, 1.0f))) - newPosition;
		m_Forward			   = glm::vec3((matrix * glm::vec4(m_Position + m_Forward, 1.0f))) - newPosition;

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