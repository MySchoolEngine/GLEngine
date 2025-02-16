#pragma once

#include <Renderer/ICameraComponent.h>

#include <Utils/BitField.h>
#include <Utils/HighResolutionTimer.h>

// Button indicator
// F.e. I have a "W" bound as a camera forward movement
// When "W" is pressed, call handleInputMessage(CAMERA_FORWARD_DOWN)
// When "W" is released (user put off the finger from "W" key), call handleInputMessage(CAMERA_FORWARD_UP)
// The UP and DOWN indicates whether the button was pressed or released
// Camera update method will handle the message
enum class CameraMessage : std::uint8_t {
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

// Free look camera
// Works similarly to FPS camera
// Timer and quaternion-based
// Uses quaternions for rotations, which can be local
// More suitable for animations - just by calling positionCamera

namespace GLEngine {
namespace Core {
class C_KeyPressedEvent;
class C_KeyReleasedEvent;
} // namespace Core

namespace Physics::Primitives {
class C_Frustum;
struct S_AABB;
} // namespace Physics::Primitives

namespace Renderer::Cameras {
class RENDERER_API_EXPORT FreelookCamera : public I_CameraComponent {
public:
	explicit FreelookCamera(const std::shared_ptr<Entity::I_Entity>& owner);
	~FreelookCamera() override; // = default;

	//=============================================================
	// Renderer::I_CameraComponent
	//=============================================================
	[[nodiscard]] glm::mat4 GetScreenToWorldMatrix() const override;
	[[nodiscard]] glm::mat4 GetViewProjectionMatrix() const override;
	[[nodiscard]] glm::mat4 GetProjectionMatrix() const override;
	[[nodiscard]] glm::mat4 GetViewMatrix() const override;
	[[nodiscard]] glm::quat GetRotation() const override;
	[[nodiscard]] glm::vec3 GetDirection() const override;
	[[nodiscard]] glm::vec3 GetPosition() const override;

	[[nodiscard]] Physics::Primitives::C_Frustum GetFrustum() const override;

	[[nodiscard]] Physics::Primitives::S_AABB GetAABB() const override;

	// Use these 2 function to set up the camera
	// FOVY in degrees
	void setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg);
	void positionCamera(const glm::vec3& camPosition, const glm::vec3& focusPoint, const glm::vec3& upDirection = glm::vec3(0, 1, 0));

	void handleInputMessage(CameraMessage m);
	void adjustOrientation(float hRad, float vRad);
	void setMovementSpeed(float s);
	void resetButtons();

	// Update method - call every time when camera data needs updating
	void Update() override;

	// Inherited via I_Camera
	// virtual void debugDraw() const override;

	[[nodiscard]] float getMovementSpeed() const;

	// from I_Camera
	[[nodiscard]] float			GetFar() const override;
	virtual void				SetFar(float Far);
	[[nodiscard]] float			GetNear() const override;
	virtual void				SetNear(float Near);
	[[nodiscard]] virtual float GetFov() const;
	virtual void				SetFov(float fov);
	[[nodiscard]] virtual float GetAspectRatio() const;

	//=================================================================================
	void						   DebugDrawGUI() override;
	[[nodiscard]] bool			   HasDebugDrawGUI() const override;
	[[nodiscard]] std::string_view GetDebugComponentName() const override;
	void						   OnEvent(Core::I_Event& event) override;

protected:
	[[nodiscard]] bool OnKeyPressed(Core::C_KeyPressedEvent& event);
	[[nodiscard]] bool OnKeyReleased(Core::C_KeyReleasedEvent& event);

private:
	void  CreateProjection();
	float _cameraMovementSpeed;

	enum class CameraDirectionFlags {
		CAMERA_FORWARD_BIT	= 1,
		CAMERA_BACKWARD_BIT = 2,
		CAMERA_LEFT_BIT		= 4,
		CAMERA_RIGHT_BIT	= 8,
		CAMERA_UP_BIT		= 16,
		CAMERA_DOWN_BIT		= 32
	};

	Utils::C_BitField<CameraDirectionFlags> m_Flags;
	float									_yaw, _pitch; // camera angles

	Utils::HighResolutionTimer _timer;

	glm::vec3 _position;
	glm::vec3 _up;
	glm::vec3 _left;
	glm::vec3 _view;

	glm::quat m_rotation;

	float m_nearZ;
	float m_farZ;
	float m_fov;

	float m_aspectRatio;

	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;
	glm::mat4 _ScreenToWorld;
};
} // namespace Renderer::Cameras
} // namespace GLEngine