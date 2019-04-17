#include <GLRendererStdafx.h>

#include <GLRenderer/Cameras/OrbitalCamera.h>

#include <Core/Application.h>
#include <Core/IWindowManager.h>
#include <Core/IWindow.h>
#include <Core/Input.h>
#include <Core/EventSystem/EventDispatcher.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp> 

#include <glm/glm.hpp> 

#include <stdexcept>

namespace GLEngine {
namespace GLRenderer {
namespace Cameras {

//=================================================================================
C_OrbitalCamera::C_OrbitalCamera()
	: m_ControlSpeed(0.5f)
{
	_pos = _view = _up = _left = glm::vec3(0);
	_zoom = _angleXDeg = _angleYDeg = 0.0f;
}

//=================================================================================
void C_OrbitalCamera::setupCameraView(float zoom, glm::vec3 center, float angleXDeg, float angleYDeg)
{
	_zoom = zoom;

	_center = center;
	_angleXDeg = angleXDeg;
	_angleYDeg = angleYDeg;
}

//=================================================================================
void C_OrbitalCamera::setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovY)
{
	_nearZ = (nearZ);
	_farZ = (farZ);
	_aspect = aspectRatio;
	_fovy = glm::radians(fovY);
}

//=================================================================================
void C_OrbitalCamera::adjustOrientation(float dx, float dy)
{
	_angleXDeg += dx;
	_angleYDeg += dy;
	_angleYDeg = glm::max(glm::min(_angleYDeg, 89.0f), -89.0f);
}

//=================================================================================
void C_OrbitalCamera::adjustZoom(int d)
{
	float c = 1.1f;
	_zoom = d < 0 ? _zoom*c : _zoom / c;
}

//=================================================================================
void C_OrbitalCamera::setCenterPoint(const glm::vec3& center)
{
	_center = center;
}

//=================================================================================
void C_OrbitalCamera::update()
{
	float radx = glm::radians(_angleXDeg);
	float rady = glm::radians(_angleYDeg);

	float x = _zoom * cos(rady) * cos(radx);
	float y = _zoom * sin(rady);
	float z = _zoom * cos(rady) * sin(radx);

	_pos = glm::vec3(x, y, z) + _center;
	_view = _center - _pos;
	_left = glm::cross(glm::vec3(0, 1, 0), glm::normalize(_view));
	_up = glm::cross(_view, _left);

	/*
	_pos = glm::vec3(x, y, z);
	_pos += _center;
	_up = glm::vec3(0, 1, 0);
	*/
	_viewMatrix = glm::lookAt(_pos, _center, _up);
	_projectionMatrix = glm::perspective(_fovy, _aspect, GetNear(), GetFar());
}

//=================================================================================
float C_OrbitalCamera::GetFov() const
{
	return _fovy;
}

//=================================================================================
void C_OrbitalCamera::OnEvent(Core::I_Event& event)
{
	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_KeyPressedEvent>(std::bind(&C_OrbitalCamera::OnKeyPressed, this, std::placeholders::_1));
	d.Dispatch<Core::C_KeyRepeatedEvent>(std::bind(&C_OrbitalCamera::OnKeyRepeated, this, std::placeholders::_1));
	d.Dispatch<Core::C_MouseScrollEvent>(std::bind(&C_OrbitalCamera::OnMouseScroll, this, std::placeholders::_1));
	d.Dispatch<Core::C_MouseButtonPressed>(std::bind(&C_OrbitalCamera::OnMousePress, this, std::placeholders::_1));

}

//=================================================================================
bool C_OrbitalCamera::OnKeyEvent(Core::C_KeyEvent& event)
{

	//===============================================
	// Rotations
	if (event.GetKeyCode() == GLFW_KEY_DOWN) {
		adjustOrientation(0.0f, -m_ControlSpeed);
		update();
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_UP) {
		adjustOrientation(0.0f, m_ControlSpeed);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_LEFT) {
		adjustOrientation(m_ControlSpeed, 0.0f);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_RIGHT) {
		adjustOrientation(-m_ControlSpeed, 0.0f);
		return true;
	}

	//===============================================
	// Position
	glm::vec3 normalToPlane = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 forwardProjOnXZ = glm::normalize(_view - glm::proj(_view, normalToPlane)) * m_ControlSpeed;
	glm::vec4 left = glm::rotate(glm::vec4(forwardProjOnXZ, 1.0f), -glm::half_pi<float>(), normalToPlane);
	glm::vec3 leftProjOnXZ = glm::vec3(left.x, 0.0f, left.z);
	if (event.GetKeyCode() == GLFW_KEY_W) {
		glm::vec3 normalToPlane = glm::vec3(0.0f, 1.0f, 0.0f);
		_center += forwardProjOnXZ;
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_S) {
		glm::vec3 normalToPlane = glm::vec3(0.0f, 1.0f, 0.0f);
		_center -= forwardProjOnXZ;
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_A) {
		_center -= leftProjOnXZ;
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_D) {
		_center += leftProjOnXZ;
		return true;
	}
	return false;
}

//=================================================================================
bool C_OrbitalCamera::OnKeyPressed(Core::C_KeyPressedEvent& event)
{
	if (OnKeyEvent(event)) {
		return true;
	}

	//===============================================
	// Zoom
	if (event.GetKeyCode() == GLFW_KEY_MINUS || event.GetKeyCode()==GLFW_KEY_KP_SUBTRACT) {
		adjustZoom(-2);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_KP_ADD) {
		adjustZoom(+2);
		return true;
	}
	return false;
}

//=================================================================================
bool C_OrbitalCamera::OnKeyRepeated(Core::C_KeyRepeatedEvent& event)
{
	if (OnKeyEvent(event)) {
		return true;
	}
	return false;
}

//=================================================================================
bool C_OrbitalCamera::OnMouseScroll(Core::C_MouseScrollEvent& event)
{
	adjustZoom(static_cast<int>(-event.GetYOffset()*10));
	return true;
}

//=================================================================================
bool C_OrbitalCamera::OnMousePress(Core::C_MouseButtonPressed& event)
{
	auto remapFnc = [](float low1, float high1, float low2, float high2, float value) {
		return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
	};
	if (event.GetMouseButton() == 0) {
		auto window = Core::C_Application::Get().GetWndMgr().GetWindow(event.GetWindowGUID());
		auto screenCoord = window->GetInput().GetMousePosition();
		auto ViewProjectionMat = GetViewProjectionMatrix();
		auto inverseVPMat = glm::inverse(ViewProjectionMat);

		float normX = remapFnc(0, window->GetWidth(), -1, 1, screenCoord.first);
		float normY = remapFnc(0, window->GetHeight(), -1, 1, screenCoord.second);

		glm::vec4 start(normX, normY, 0.99999999f, 1.0f);
		start = inverseVPMat* start;


		// move center
		auto dv = _view;
		float k = -(start.y / dv.y);
		_center.x = start.x+k*dv.x;
		_center.y = 0.0f;
		_center.z = start.z + k*dv.z;

		return true;
	}
	return false;
}

//=================================================================================
float C_OrbitalCamera::GetAspectRatio() const
{
	return _aspect;
}

//=================================================================================
glm::quat C_OrbitalCamera::GetRotation() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
glm::vec3 C_OrbitalCamera::GetDirection() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetViewProjectionMatrix() const
{
	return _projectionMatrix * _viewMatrix;
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetProjectionMatrix() const
{
	return _projectionMatrix;
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetViewMatrix() const
{
	return _viewMatrix;
}

//=================================================================================
glm::vec3 C_OrbitalCamera::GetPosition() const
{
	return _pos;
}

//=================================================================================
GLEngine::Physics::Primitives::C_Frustum C_OrbitalCamera::GetFrustum() const
{
	return GLEngine::Physics::Primitives::C_Frustum(GetPosition(), _up, _center - GetPosition(), GetNear(), GetFar(), GetAspectRatio(), GetFov());
}

}}}