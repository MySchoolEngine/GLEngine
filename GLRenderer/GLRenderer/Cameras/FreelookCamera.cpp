#include "FreelookCamera.hpp"

#include "CameraPath.h"

#include "Frustum.h"

#include <glm/gtx/norm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Application.hpp>
#include <CameraManager.h>
#include <Debug.h>

//=================================================================================
FreelookCamera::FreelookCamera()
	: m_KeyLogger(10000.0f)
{
    _cameraMovementSpeed = 1;
    _yaw = _pitch = 0;
    _flags = 0;
	m_rotation = glm::quat();

    _position = _left = _up = glm::vec3(0);

    _viewMatrix = _projectionMatrix = glm::mat4(1);

    _timer.reset();
}

//=================================================================================
void FreelookCamera::setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg)
{
	m_farZ = farZ;
	m_nearZ = nearZ;
	m_fov = fovYDeg;
	m_aspectRatio = aspectRatio; 
	CreateProjection();
}

//=================================================================================
void FreelookCamera::setMovementSpeed(float s)
{
    if(s>0)
        _cameraMovementSpeed = s;
}

//=================================================================================
void FreelookCamera::adjustOrientation(float hDeg, float vDeg)
{
    _yaw += glm::radians(hDeg);
    _pitch += glm::radians(vDeg);
    
	//value clamping - keep heading and pitch between 0 and 2 pi
    if ( _yaw > glm::two_pi<float>() ) 
		_yaw -= glm::two_pi<float>();
	else if ( _yaw < 0 ) 
		_yaw = glm::two_pi<float>() + _yaw;
	
	if ( _pitch > glm::two_pi<float>() ) 
		_pitch -= glm::two_pi<float>();
	else if ( _pitch < 0 ) 
		_pitch = glm::two_pi<float>() + _pitch;
    //*/
}

//=================================================================================
void FreelookCamera::resetButtons()
{
    _flags = 0;
}

//=================================================================================
void FreelookCamera::LogKeyFrame()
{
	m_KeyLogger.LogKeyFrame(_position, _view, _up);
}

//=================================================================================
void FreelookCamera::DumpKeyFrames() const
{
	m_KeyLogger.DumpKeyFrames();
}

//=================================================================================
void FreelookCamera::positionCamera(const glm::vec3 &camPosition, const glm::vec3& focusPoint, const glm::vec3 &upDirection)
{
    _position = camPosition;
    _up = upDirection;
    _view = glm::normalize(focusPoint - _position);

    _left = glm::normalize(glm::cross(_up, _view));
}

//=================================================================================
void FreelookCamera::positionCamera(const CameraPathKeypoint& key)
{
	_position = key.position;
	_up		  = key.upVector;
	_view	  = key.viewVector;
}

//=================================================================================
void FreelookCamera::handleInputMessage( CameraMessage msg )
{
    switch (msg)
    {
    case CameraMessage::CAMERA_FORWARD_DOWN:
        SetBit(_flags, CAMERA_FORWARD_BIT);
        break;
    case CameraMessage::CAMERA_FORWARD_UP:
        ClearBit(_flags, CAMERA_FORWARD_BIT);
        break;

    case CameraMessage::CAMERA_BACKWARD_DOWN:
        SetBit(_flags, CAMERA_BACKWARD_BIT);
        break;
    case CameraMessage::CAMERA_BACKWARD_UP:
        ClearBit(_flags, CAMERA_BACKWARD_BIT);
        break;

    case CameraMessage::CAMERA_LEFT_DOWN:
        SetBit(_flags, CAMERA_LEFT_BIT);
        break;
    case CameraMessage::CAMERA_LEFT_UP:
        ClearBit(_flags, CAMERA_LEFT_BIT);
        break;

    case CameraMessage::CAMERA_RIGHT_DOWN:
        SetBit(_flags, CAMERA_RIGHT_BIT);
        break;
    case CameraMessage::CAMERA_RIGHT_UP:
        ClearBit(_flags, CAMERA_RIGHT_BIT);
        break;

    case CameraMessage::CAMERA_UP_DOWN:
        SetBit(_flags, CAMERA_UP_BIT);
        break;
    case CameraMessage::CAMERA_UP_UP:
        ClearBit(_flags, CAMERA_UP_BIT);
        break;

    case CameraMessage::CAMERA_DOWN_DOWN:
        SetBit(_flags, CAMERA_DOWN_BIT);
        break;
    case CameraMessage::CAMERA_DOWN_UP:
        ClearBit(_flags, CAMERA_DOWN_BIT);
        break;

    default:
        break;
    }
}

//=================================================================================
void FreelookCamera::update()
{
    float t = (float)_timer.getElapsedTimeFromLastQuerySeconds();
    
    glm::mat4 m = glm::mat4(1);
	if (_yaw != 0 || _pitch != 0)
	{
		glm::quat p = glm::angleAxis(_pitch, _left);
		glm::quat q = glm::angleAxis(-_yaw, glm::vec3(0, 1, 0));

		m = glm::mat4_cast(glm::normalize(q*p));
		m_rotation = glm::normalize(m_rotation * glm::normalize(q*p));
	}

    _view = glm::normalize(glm::vec3(m * glm::vec4(_view, 0)));
    _up = glm::normalize(glm::vec3(m * glm::vec4(_up, 0)));
    _left = glm::cross(_up, _view);

    //Move camera position
    if (IsBitSet(_flags, CAMERA_FORWARD_BIT) ) 
        _position += _cameraMovementSpeed * t * _view;	//forward
    if (IsBitSet(_flags, CAMERA_BACKWARD_BIT)) 
        _position -= _cameraMovementSpeed * t * _view;	//back
    if (IsBitSet(_flags, CAMERA_LEFT_BIT))     
        _position += _cameraMovementSpeed * t * _left; //left
    if (IsBitSet(_flags, CAMERA_RIGHT_BIT))    
        _position -= _cameraMovementSpeed * t * _left; //right
    if (IsBitSet(_flags, CAMERA_UP_BIT))       
        _position += _cameraMovementSpeed * t * glm::vec3(0, 1, 0); //up
    if (IsBitSet(_flags, CAMERA_DOWN_BIT))     
        _position -= _cameraMovementSpeed * t * glm::vec3(0, 1, 0); //down

    //Update view matrix
    _viewMatrix = glm::lookAt(_position, _position + _view, _up);

    //Reset angles
    _pitch = 0.0f;
	_yaw = 0.0f;
}

//=================================================================================
glm::vec3 FreelookCamera::getPosition() const
{
    return _position;
}

//=================================================================================
glm::mat4 FreelookCamera::getViewProjectionMatrix() const
{
    return _projectionMatrix * _viewMatrix;
}

//=================================================================================
glm::mat4 FreelookCamera::getViewMatrix() const
{
    return _viewMatrix;
}

//=================================================================================
glm::mat4 FreelookCamera::getProjectionMatrix() const
{
    return _projectionMatrix;
}

//=================================================================================
glm::quat FreelookCamera::getRotation() const
{
	//glm::quat rot1 = glm::normalize(glm::quat(glm::vec3(0.0f, 0.0f, 1.0f), glm::normalize(_view - _position)));
	//
	//glm::vec3 right = glm::cross(_view, glm::vec3(0.0f,1.0f,0.0f));
	//
	//glm::vec3 up = cross(right, _view - _position);
	//
	//glm::vec3 newUp = rot1 * glm::vec3(0.0f, 1.0f, 0.0f);
	//
	//glm::quat rot2 = glm::normalize(glm::quat(newUp, up));



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
bool FreelookCamera::Input(SDL_Event event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
	{
		if (event.key.keysym.sym == SDLK_w)
			handleInputMessage(CameraMessage::CAMERA_FORWARD_DOWN);
		else if (event.key.keysym.sym == SDLK_s)
			handleInputMessage(CameraMessage::CAMERA_BACKWARD_DOWN);
		else if (event.key.keysym.sym == SDLK_a)
			handleInputMessage(CameraMessage::CAMERA_LEFT_DOWN);
		else if (event.key.keysym.sym == SDLK_d)
			handleInputMessage(CameraMessage::CAMERA_RIGHT_DOWN);
		else if (event.key.keysym.sym == SDLK_m)
			DumpKeyFrames();
		else if (event.key.keysym.sym == SDLK_k)
			LogKeyFrame();
		break;
	}

	case SDL_KEYUP:
	{
		if (event.key.keysym.sym == SDLK_w)
			handleInputMessage(CameraMessage::CAMERA_FORWARD_UP);
		else if (event.key.keysym.sym == SDLK_s)
			handleInputMessage(CameraMessage::CAMERA_BACKWARD_UP);
		else if (event.key.keysym.sym == SDLK_a)
			handleInputMessage(CameraMessage::CAMERA_LEFT_UP);
		else if (event.key.keysym.sym == SDLK_d)
			handleInputMessage(CameraMessage::CAMERA_RIGHT_UP);
		break;
	}
	default:
		break;
	}
	return true;
}

//=================================================================================
AABB FreelookCamera::GetAABB() const
{
	C_Frustum frust(_position, _up, _view, m_nearZ, m_farZ, GetAspectRatio(), GetFov());
	return frust.GetAABB();
}

//=================================================================================
C_Frustum FreelookCamera::getFrustum() const
{
	return C_Frustum(_position, _up, _view, m_nearZ, m_farZ, m_aspectRatio, m_fov);
}

//=================================================================================
void FreelookCamera::CreateProjection()
{
	_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearZ, m_farZ);
}

//=================================================================================
void FreelookCamera::SetFar(float far)
{
	m_farZ = far;
	CreateProjection();
}

//=================================================================================
void FreelookCamera::SetNear(float near)
{
	m_nearZ = near;
	CreateProjection();
}

//=================================================================================
void FreelookCamera::SetFov(float fov)
{
	m_fov = fov;
	CreateProjection();
}

//=================================================================================
glm::vec3 FreelookCamera::getDirection() const
{
	return _view;
}

//=================================================================================
void FreelookCamera::debugDraw() const
{
	C_Frustum frust(_position, _up, _view, m_nearZ, m_farZ, GetAspectRatio(), GetFov());
	frust.DebugDraw(glm::vec3(0.0f,1.0f,0.0f));
	C_DebugDraw::Instance().DrawAABB(frust.GetAABB(), Application::Instance().GetCamManager()->GetActiveCamera()->getViewProjectionMatrix());
}
