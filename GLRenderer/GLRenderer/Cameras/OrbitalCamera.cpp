#include <GLRendererStdafx.h>

#include <GLRenderer/Cameras/OrbitalCamera.h>

#include <GLRenderer/PersistentDebug.h>

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
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdexcept>

namespace GLEngine {
namespace GLRenderer {
namespace Cameras {

//=================================================================================
C_OrbitalCamera::C_OrbitalCamera()
	: m_ControlSpeed(0.5f)
	, GLE_DEBUG_MEMBER_CTOR_LIST((0.0f, -89.0f, 89.0f, "Y angle:"), (0.0f), _angleYDeg)
	, GLE_DEBUG_MEMBER_CTOR_LIST((0.0f, 0, 360.f, "X angle:"), (0.0f), _angleXDeg)
	, GLE_DEBUG_MEMBER_CTOR_LIST((0.0f, 0.1f, 50.f, "Zoom:"), (0.0f), _zoom)
{
	_pos = _view = _up = _left = glm::vec3(0);
}

//=================================================================================
C_OrbitalCamera::~C_OrbitalCamera() = default;

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
	_angleYDeg = glm::max(glm::min(static_cast<float>(_angleYDeg), 89.0f), -89.0f);
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
void C_OrbitalCamera::DebugDraw()
{
	C_DebugDraw::Instance().DrawPoint(_center, glm::vec3(0, 0, 1), glm::mat4(1.0f));
}

//=================================================================================
void C_OrbitalCamera::Update()
{
	float radx = glm::radians(static_cast<float>(_angleXDeg));
	float rady = glm::radians(static_cast<float>(_angleYDeg));

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
void C_OrbitalCamera::DebugDrawGUI()
{
#ifdef GL_ENGINE_DEBUG
	if (::ImGui::CollapsingHeader("Orbital camera")) {
		_angleYDeg.Draw();
		_angleXDeg.Draw();
		_zoom.Draw();
	}
#endif
}

//=================================================================================
bool C_OrbitalCamera::OnKeyEvent(Core::C_KeyEvent& event)
{

	//===============================================
	// Rotations
	if (event.GetKeyCode() == GLFW_KEY_DOWN) {
		adjustOrientation(0.0f, -m_ControlSpeed);
		Update();
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

		float x = (2.0f * screenCoord.first) / window->GetWidth() - 1.0f;
		float y = 1.0f - (2.0f * screenCoord.second) / window->GetHeight();
		float z = 1.0f;
		glm::vec3 ray_nds = glm::vec3(x, y, z);
		glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
		glm::vec4 ray_eye = glm::inverse(GetProjectionMatrix()) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

		glm::vec4 ray_wor = glm::inverse(GetViewMatrix()) * ray_eye;
		// don't forget to normalise the vector at some point
		ray_wor = glm::normalize(ray_wor);
		C_PersistentDebug::Instance().DrawLine(glm::vec4(_pos, 1.0f), glm::vec4(_pos, 1.0f) + ray_wor, glm::vec3(0, 1, 0));



		glm::vec4 planeNormal = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		float t = -(glm::dot(glm::vec4(_pos, 1.0f),planeNormal) - 1 /*offset from origin*/) / (glm::dot(ray_wor, planeNormal));
		if (t > 0) {
			glm::vec4 hit = glm::vec4(_pos, 1.0f) + ray_wor*t;
			C_PersistentDebug::Instance().DrawPoint(hit, glm::vec3(1, 0, 0), glm::mat4(1.0f));

			_center = hit;
			Update();
			return true;
		}
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
	return _center - GetPosition();
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
	return GLEngine::Physics::Primitives::C_Frustum(GetPosition(), _up, GetDirection(), GetNear(), GetFar(), GetAspectRatio(), GetFov());
}

}}}