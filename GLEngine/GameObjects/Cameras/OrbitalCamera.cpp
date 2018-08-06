#include "OrbitalCamera.h"

#include "Physics/Frustum.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>


namespace GameObjects {
namespace Cameras {


//=================================================================================
constexpr C_OrbitalCamera::C_OrbitalCamera(glm::vec3 center)
	: m_Center(center)
	, m_Zoom(10.0f)
	, m_AngleXDeg(0.0f)
	, m_AngleYDeg(0.0f)
{
	Update();
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetViewProjectionMatrix() const
{
	return m_ProjectionMatrix * m_ViewMatrix;
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

//=================================================================================
glm::quat C_OrbitalCamera::GetRotation() const
{
	throw std::logic_error("The method or operation is not implemented.");
	return glm::quat();
}

//=================================================================================
glm::vec3 C_OrbitalCamera::GetDirection() const
{
	return m_Center - m_Pos;
}

//=================================================================================
glm::vec3 C_OrbitalCamera::GetPosition() const
{
	return m_Pos;
}

//=================================================================================
Shapes::C_Frustum C_OrbitalCamera::GetFrustum() const
{
	return Shapes::C_Frustum(m_Pos, m_Up, GetDirection(), m_NearZ, m_FarZ, m_Aspect, m_FovY);
}

//=================================================================================
void C_OrbitalCamera::setupCameraView(float zoom, glm::vec3 center, float angleXDeg, float angleYDeg)
{
	m_Zoom		= zoom;
	m_Center	= center;
	m_AngleXDeg = angleXDeg;
	m_AngleYDeg = angleYDeg;
	Update();
}

//=================================================================================
void C_OrbitalCamera::setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg)
{
	m_NearZ		= nearZ;
	m_FarZ		= farZ;
	m_Aspect	= aspectRatio;
	m_FovY		= glm::radians(fovYDeg);

	Update();
}

//=================================================================================
void C_OrbitalCamera::Update()
{
	float radx = glm::radians(m_AngleXDeg);
	float rady = glm::radians(m_AngleYDeg);

	float x = m_Zoom * cos(rady) * cos(radx);
	float y = m_Zoom * sin(rady);
	float z = m_Zoom * cos(rady) * sin(radx);

	m_Pos = glm::vec3(x, y, z) + GetCenter();
	m_View = GetCenter() - m_Pos;
	m_Left = glm::cross(glm::vec3(0, 1, 0), glm::normalize(m_View));
	m_Up = glm::cross(m_View, m_Left);

	m_ViewMatrix = glm::lookAt(m_Pos, m_Center, m_Up);
	m_ProjectionMatrix = glm::perspective(m_FovY, m_Aspect, m_NearZ, m_FarZ);
}

}}