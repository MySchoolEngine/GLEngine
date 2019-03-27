#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

//#include "HighResolutionTimer.hpp"

//#include "CameraKeysLogger.h"


#include <Renderer/ICameraComponent.h>


//Button indicator
//F.e. I have a "W" bound as a camera forward movement
//When "W" is pressed, call handleInputMessage(CAMERA_FORWARD_DOWN)
//When "W" is released (user put off the finger from "W" key), call handleInputMessage(CAMERA_FORWARD_UP)
//The UP and DOWN indicates whether the button was pressed or released
//Camera update method will handle the message
enum class CameraMessage : unsigned char
{
    CAMERA_FORWARD_UP = 0,
    CAMERA_FORWARD_DOWN,
    CAMERA_BACKWARD_UP,
    CAMERA_BACKWARD_DOWN,
    CAMERA_LEFT_UP,
    CAMERA_LEFT_DOWN,
    CAMERA_RIGHT_UP,
    CAMERA_RIGHT_DOWN,
    CAMERA_UP_UP,
    CAMERA_UP_DOWN,
    CAMERA_DOWN_UP,
    CAMERA_DOWN_DOWN
};

class CameraPathKeypoint;

//Free look camera
//Works similarly to FPS camera
//Timer and quaternion-based
//Uses quaternions for rotations, which can be local
//More suitable for animations - just by calling positionCamera

namespace GLEngine {

namespace Physics {
namespace Primitives {
class C_Frustum;
struct S_AABB;
}
}

namespace GLRenderer {
namespace Cameras {
class FreelookCamera : public Renderer::I_CameraComponent
{
public:
	FreelookCamera();
	~FreelookCamera() = default;

	//=============================================================
	// Renderer::I_CameraComponent
	//=============================================================
	virtual glm::mat4 GetViewProjectionMatrix() const override;
	virtual glm::mat4 GetProjectionMatrix() const override;
	virtual glm::mat4 GetViewMatrix() const override;
	virtual glm::quat GetRotation() const override;
	virtual glm::vec3 GetDirection() const override;
	virtual glm::vec3 GetPosition() const override;

	virtual Physics::Primitives::C_Frustum GetFrustum()	const override;

	virtual Physics::Primitives::S_AABB GetAABB() const;

	//Use these 2 function to setup the camera
	//FOVY in degrees
	void setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg);
	void positionCamera(const glm::vec3 &camPosition, const glm::vec3& focusPoint, const glm::vec3 &upDirection = glm::vec3(0, 1, 0));
	//virtual void positionCamera(const CameraPathKeypoint& key) override;

	//void handleInputMessage(CameraMessage m);
	void adjustOrientation(float hRad, float vRad);
	void setMovementSpeed(float s);
	void resetButtons();
	//void LogKeyFrame();
	//void DumpKeyFrames() const;

	//Update method - call every time when camera data needs updating
	void update();

	// Inherited via I_Camera
	// virtual void debugDraw() const override;

	float     getMovementSpeed() const;

	// from I_Camera
	virtual float GetFar() const;
	virtual void  SetFar(float Far);
	virtual float GetNear() const;
	virtual void  SetNear(float Near);
	virtual float GetFov() const;
	virtual void  SetFov(float fov);
	virtual float GetAspectRatio() const;


	//virtual bool Input(SDL_Event event) override;



private:
	void CreateProjection();
	float               _cameraMovementSpeed;
	float               _yaw, _pitch;   //camera angles
	unsigned int        _flags;

	//HighResolutionTimer _timer;

	glm::vec3           _position;
	glm::vec3           _up;
	glm::vec3           _left;
	glm::vec3           _view;

	glm::quat			m_rotation;

	float				m_nearZ;
	float				m_farZ;
	float				m_fov;

	float				m_aspectRatio;

	glm::mat4           _viewMatrix;
	glm::mat4           _projectionMatrix;

	//C_CameraKeysLogger	m_KeyLogger;

	enum
	{
		CAMERA_FORWARD_BIT = 0,
		CAMERA_BACKWARD_BIT,
		CAMERA_LEFT_BIT,
		CAMERA_RIGHT_BIT,
		CAMERA_UP_BIT,
		CAMERA_DOWN_BIT
	};
};
}}}