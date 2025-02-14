#pragma once

#include <Renderer/ICameraComponent.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine {

namespace Core {
class C_KeyPressedEvent;
class C_MouseScrollEvent;
class C_MouseButtonPressed;
class C_KeyRepeatedEvent;
class C_KeyEvent;
} // namespace Core

namespace Physics::Primitives {
class C_Frustum;
}

namespace Renderer::Cameras {

// Orbital camera
// Using Euclidean angles (= has gimbal lock)
class RENDERER_API_EXPORT C_OrbitalCamera : public I_CameraComponent {
public:
	C_OrbitalCamera(const std::shared_ptr<Entity::I_Entity>& owner);
	C_OrbitalCamera();
	~C_OrbitalCamera() override; // = default;

	[[nodiscard]] glm::mat4 GetViewProjectionMatrix() const override;
	[[nodiscard]] glm::mat4 GetProjectionMatrix() const override;
	[[nodiscard]] glm::mat4 GetViewMatrix() const override;
	[[nodiscard]] glm::quat GetRotation() const override;
	[[nodiscard]] glm::vec3 GetDirection() const override;
	[[nodiscard]] glm::vec3 GetPosition() const override;

	[[nodiscard]] Physics::Primitives::C_Frustum GetFrustum() const override;

	[[nodiscard]] float GetAspectRatio() const;

	void SetupCameraView(const float zoom, const glm::vec3 center, const float angleXDeg, const float angleYDeg);
	void SetupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg);
	void AdjustZoom(int zoom);

	void AdjustOrientation(float dx, float dy);

	void DebugDraw();
	void Update() override;


	[[nodiscard]] float GetFar() const override { return _farZ; }
	[[nodiscard]] float GetNear() const override { return _nearZ; }

	[[nodiscard]] float GetFov() const;


	//=====================================================
	// I_Component
	//=====================================================
	void			 OnEvent(Core::I_Event& event) override;
	void			 DebugDrawGUI() override;
	bool			 HasDebugDrawGUI() const override;
	std::string_view GetDebugComponentName() const override;

	glm::mat4 GetScreenToWorldMatrix() const override;

	// Deserializer special method
	void AfterDeserialize();

	RTTR_ENABLE(I_CameraComponent);
	RTTR_REGISTRATION_FRIEND;

protected:
	bool OnKeyPressed(Core::C_KeyPressedEvent& event);
	bool OnKeyRepeated(Core::C_KeyRepeatedEvent& event);
	bool OnMouseScroll(Core::C_MouseScrollEvent& event);
	bool OnMousePress(Core::C_MouseButtonPressed& event);
	bool OnKeyEvent(Core::C_KeyEvent& event);

private:
	glm::vec3 _pos;
	glm::vec3 _view;
	glm::vec3 _left;
	glm::vec3 _up;

	float _fovy;
	float _nearZ;
	float _farZ;
	float _aspect;

	float _zoom;
	float _angleXRad;
	float _angleYRad; // radians

	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;
	glm::mat4 _ScreenToWorld;

	float m_ControlSpeed;
};
} // namespace Renderer::Cameras
} // namespace GLEngine