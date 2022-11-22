#include <RendererStdafx.h>

#include <Renderer/Cameras/FreelookCamera.h>

#include <Physics/Primitives/AABB.h>
#include <Physics/Primitives/Frustum.h>

#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/EventDispatcher.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>

#include <GLFW/glfw3.h>

namespace GLEngine::Renderer::Cameras {
//=================================================================================
FreelookCamera::FreelookCamera(std::shared_ptr<Entity::I_Entity>& owner)
	: I_CameraComponent(owner)
	, _cameraMovementSpeed(1)
	, m_Flags()
	, _yaw(0)
	, _pitch(0)
	, _position(glm::vec3(0))
	, _up(glm::vec3(0, 1, 0))
	, _left(glm::vec3(1, 0, 0))
	, _view(glm::vec3(0, 0, 1))
	, m_rotation()
	, m_nearZ(0.1f)
	, m_farZ(100.f)
	, m_fov(90.f)
	, m_aspectRatio(1900.f / 1080.f)
	, _viewMatrix(glm::lookAt(_position, _position + _view, _up))
	, _projectionMatrix(glm::mat4(1.f))
{
	CreateProjection();
	_ScreenToWorld = glm::inverse(_projectionMatrix * _viewMatrix);
}

//=================================================================================
FreelookCamera::~FreelookCamera() = default;

//=================================================================================
void FreelookCamera::setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg)
{
	m_farZ		  = farZ;
	m_nearZ		  = nearZ;
	m_fov		  = fovYDeg;
	m_aspectRatio = aspectRatio;
	CreateProjection();
}

//=================================================================================
void FreelookCamera::setMovementSpeed(float s)
{
	if (s > 0)
		_cameraMovementSpeed = s;
}

//=================================================================================
void FreelookCamera::adjustOrientation(float hDeg, float vDeg)
{
	_yaw += glm::radians(hDeg);
	_pitch += glm::radians(vDeg);

	// value clamping - keep heading and pitch between 0 and 2 pi
	if (_yaw > glm::two_pi<float>())
		_yaw -= glm::two_pi<float>();
	else if (_yaw < 0)
		_yaw = glm::two_pi<float>() + _yaw;

	if (_pitch > glm::two_pi<float>())
		_pitch -= glm::two_pi<float>();
	else if (_pitch < 0)
		_pitch = glm::two_pi<float>() + _pitch;
}

//=================================================================================
void FreelookCamera::resetButtons()
{
	m_Flags = 0;
}

//=================================================================================
void FreelookCamera::positionCamera(const glm::vec3& camPosition, const glm::vec3& focusPoint, const glm::vec3& upDirection)
{
	_position = camPosition;
	_up		  = upDirection;
	_view	  = glm::normalize(focusPoint - _position);

	_left = glm::normalize(glm::cross(_up, _view));
}

//=================================================================================
void FreelookCamera::handleInputMessage(CameraMessage msg)
{
	switch (msg)
	{
	case CameraMessage::CAMERA_FORWARD_DOWN:
		m_Flags.SetFlag(CameraDirectionFlags::CAMERA_FORWARD_BIT);
		break;
	case CameraMessage::CAMERA_FORWARD_UP:
		m_Flags.ClearFlag(CameraDirectionFlags::CAMERA_FORWARD_BIT);
		break;

	case CameraMessage::CAMERA_BACKWARD_DOWN:
		m_Flags.SetFlag(CameraDirectionFlags::CAMERA_BACKWARD_BIT);
		break;
	case CameraMessage::CAMERA_BACKWARD_UP:
		m_Flags.ClearFlag(CameraDirectionFlags::CAMERA_BACKWARD_BIT);
		break;

	case CameraMessage::CAMERA_LEFT_DOWN:
		m_Flags.SetFlag(CameraDirectionFlags::CAMERA_LEFT_BIT);
		break;
	case CameraMessage::CAMERA_LEFT_UP:
		m_Flags.ClearFlag(CameraDirectionFlags::CAMERA_LEFT_BIT);
		break;

	case CameraMessage::CAMERA_RIGHT_DOWN:
		m_Flags.SetFlag(CameraDirectionFlags::CAMERA_RIGHT_BIT);
		break;
	case CameraMessage::CAMERA_RIGHT_UP:
		m_Flags.ClearFlag(CameraDirectionFlags::CAMERA_RIGHT_BIT);
		break;

	case CameraMessage::CAMERA_UP_DOWN:
		m_Flags.SetFlag(CameraDirectionFlags::CAMERA_UP_BIT);
		break;
	case CameraMessage::CAMERA_UP_UP:
		m_Flags.ClearFlag(CameraDirectionFlags::CAMERA_UP_BIT);
		break;

	case CameraMessage::CAMERA_DOWN_DOWN:
		m_Flags.SetFlag(CameraDirectionFlags::CAMERA_DOWN_BIT);
		break;
	case CameraMessage::CAMERA_DOWN_UP:
		m_Flags.ClearFlag(CameraDirectionFlags::CAMERA_DOWN_BIT);
		break;

	default:
		break;
	}
}

//=================================================================================
void FreelookCamera::Update(float dt)
{
	glm::mat4 m = glm::mat4(1);
	if (_yaw != 0 || _pitch != 0)
	{
		glm::quat p = glm::angleAxis(_pitch, _left);
		glm::quat q = glm::angleAxis(-_yaw, glm::vec3(0, 1, 0));

		m		   = glm::mat4_cast(glm::normalize(q * p));
		m_rotation = glm::normalize(m_rotation * glm::normalize(q * p));
	}

	_view = glm::normalize(glm::vec3(m * glm::vec4(_view, 0)));
	_up	  = glm::normalize(glm::vec3(m * glm::vec4(_up, 0)));
	_left = glm::cross(_up, _view);

	// Move camera position
	if (m_Flags.CheckFlag(CameraDirectionFlags::CAMERA_FORWARD_BIT))
		_position += _cameraMovementSpeed * dt * _view; // forward
	if (m_Flags.CheckFlag(CameraDirectionFlags::CAMERA_BACKWARD_BIT))
		_position -= _cameraMovementSpeed * dt * _view; // back
	if (m_Flags.CheckFlag(CameraDirectionFlags::CAMERA_LEFT_BIT))
		_position += _cameraMovementSpeed * dt * _left; // left
	if (m_Flags.CheckFlag(CameraDirectionFlags::CAMERA_RIGHT_BIT))
		_position -= _cameraMovementSpeed * dt * _left; // right
	if (m_Flags.CheckFlag(CameraDirectionFlags::CAMERA_UP_BIT))
		_position += _cameraMovementSpeed * dt * glm::vec3(0, 1, 0); // up
	if (m_Flags.CheckFlag(CameraDirectionFlags::CAMERA_DOWN_BIT))
		_position -= _cameraMovementSpeed * dt * glm::vec3(0, 1, 0); // down

	// Update view matrix
	_viewMatrix	   = glm::lookAt(_position, _position + _view, _up);
	_ScreenToWorld = glm::inverse(_projectionMatrix * _viewMatrix);

	// Reset angles
	_pitch = 0.0f;
	_yaw   = 0.0f;
}

//=================================================================================
glm::vec3 FreelookCamera::GetPosition() const
{
	return _position;
}

//=================================================================================
glm::mat4 FreelookCamera::GetViewProjectionMatrix() const
{
	return _projectionMatrix * _viewMatrix;
}

//=================================================================================
glm::mat4 FreelookCamera::GetViewMatrix() const
{
	return _viewMatrix;
}

//=================================================================================
glm::mat4 FreelookCamera::GetProjectionMatrix() const
{
	return _projectionMatrix;
}

//=================================================================================
glm::quat FreelookCamera::GetRotation() const
{
	// glm::quat rot1 = glm::normalize(glm::quat(glm::vec3(0.0f, 0.0f, 1.0f),
	// glm::normalize(_view - _position)));
	//
	// glm::vec3 right = glm::cross(_view, glm::vec3(0.0f,1.0f,0.0f));
	//
	// glm::vec3 up = cross(right, _view - _position);
	//
	// glm::vec3 newUp = rot1 * glm::vec3(0.0f, 1.0f, 0.0f);
	//
	// glm::quat rot2 = glm::normalize(glm::quat(newUp, up));

	return m_rotation;
}

//=================================================================================
float FreelookCamera::getMovementSpeed() const
{
	return _cameraMovementSpeed;
}

//=================================================================================
float FreelookCamera::GetFar() const
{
	return m_farZ;
}

//=================================================================================
float FreelookCamera::GetNear() const
{
	return m_nearZ;
}

//=================================================================================
float FreelookCamera::GetFov() const
{
	return m_fov;
}

//=================================================================================
float FreelookCamera::GetAspectRatio() const
{
	return m_aspectRatio;
}

//=================================================================================
void FreelookCamera::DebugDrawGUI()
{
}

//=================================================================================
std::string_view FreelookCamera::GetDebugComponentName() const
{
	return "Freelook camera";
}

//=================================================================================
bool FreelookCamera::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
Physics::Primitives::S_AABB FreelookCamera::GetAABB() const
{
	Physics::Primitives::C_Frustum frust(_position, _up, _view, m_nearZ, m_farZ, GetAspectRatio(), GetFov());
	return frust.GetAABB();
}

//=================================================================================
Physics::Primitives::C_Frustum FreelookCamera::GetFrustum() const
{
	return Physics::Primitives::C_Frustum(_position, _up, _view, m_nearZ, m_farZ, m_aspectRatio, m_fov);
}

//=================================================================================
void FreelookCamera::CreateProjection()
{
	_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearZ, m_farZ);
	_ScreenToWorld	  = glm::inverse(_projectionMatrix * _viewMatrix);
}

//=================================================================================
void FreelookCamera::SetFar(float Far)
{
	m_farZ = Far;
	CreateProjection();
}

//=================================================================================
void FreelookCamera::SetNear(float Near)
{
	m_nearZ = Near;
	CreateProjection();
}

//=================================================================================
void FreelookCamera::SetFov(float fov)
{
	m_fov = fov;
	CreateProjection();
}

//=================================================================================
glm::vec3 FreelookCamera::GetDirection() const
{
	return _view;
}

//=================================================================================
void FreelookCamera::OnEvent(Core::I_Event& event)
{
	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_KeyPressedEvent>(std::bind(&FreelookCamera::OnKeyPressed, this, std::placeholders::_1));
	d.Dispatch<Core::C_KeyReleasedEvent>(std::bind(&FreelookCamera::OnKeyReleased, this, std::placeholders::_1));
}

//=================================================================================
bool FreelookCamera::OnKeyPressed(Core::C_KeyPressedEvent& event)
{
	if (event.GetKeyCode() == GLFW_KEY_DOWN)
	{
		handleInputMessage(CameraMessage::CAMERA_BACKWARD_DOWN);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_UP)
	{
		handleInputMessage(CameraMessage::CAMERA_FORWARD_DOWN);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_LEFT)
	{
		handleInputMessage(CameraMessage::CAMERA_LEFT_DOWN);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_RIGHT)
	{
		handleInputMessage(CameraMessage::CAMERA_RIGHT_DOWN);
	}
	return false;
}

//=================================================================================
bool FreelookCamera::OnKeyReleased(Core::C_KeyReleasedEvent& event)
{
	if (event.GetKeyCode() == GLFW_KEY_DOWN)
	{
		handleInputMessage(CameraMessage::CAMERA_BACKWARD_UP);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_UP)
	{
		handleInputMessage(CameraMessage::CAMERA_FORWARD_UP);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_LEFT)
	{
		handleInputMessage(CameraMessage::CAMERA_LEFT_UP);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_RIGHT)
	{
		handleInputMessage(CameraMessage::CAMERA_RIGHT_UP);
	}
	return false;
}

//=================================================================================
glm::mat4 FreelookCamera::GetScreenToworldMatrix() const
{
	return _ScreenToWorld;
}

} // namespace GLEngine::Renderer::Cameras
