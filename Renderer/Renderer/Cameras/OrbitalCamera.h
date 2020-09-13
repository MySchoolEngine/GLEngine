#pragma once

#include <Renderer/ICameraComponent.h>

#include <GUI/Input/Slider.h>

#include <Core/CoreMacros.h>

namespace GLEngine {

namespace Core {
class C_KeyPressedEvent;
class C_MouseScrollEvent;
class C_MouseButtonPressed;
class C_KeyRepeatedEvent;
class C_KeyEvent;
}

namespace Physics::Primitives {
class C_Frustum;
}

namespace Renderer::Cameras {

//Orbital camera
//Using Euclidean angles (= has gimbal lock)
class RENDERER_API_EXPORT C_OrbitalCamera : public I_CameraComponent
{
public:
	C_OrbitalCamera();
	~C_OrbitalCamera();

	[[nodiscard]] virtual glm::mat4 GetViewProjectionMatrix() const override;
	[[nodiscard]] virtual glm::mat4 GetProjectionMatrix() const override;
	[[nodiscard]] virtual glm::mat4 GetViewMatrix() const override;
	[[nodiscard]] virtual glm::quat GetRotation() const override;
	[[nodiscard]] virtual glm::vec3 GetDirection() const override;
	[[nodiscard]] virtual glm::vec3 GetPosition() const override;

	[[nodiscard]] virtual Physics::Primitives::C_Frustum GetFrustum()	const override;


	[[nodiscard]] float GetAspectRatio() const;

	void setupCameraView(float zoom, glm::vec3 center, float angleXDeg, float angleYDeg);
	void setCenterPoint(const glm::vec3& center);
	void setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg);
	void adjustZoom(int zoom);

	void adjustOrientation(float dx, float dy);

	void DebugDraw();
	virtual void Update() override;


	[[nodiscard]] float GetFar() const { return _farZ; }
	[[nodiscard]] float GetNear() const { return _nearZ; }

	[[nodiscard]] float GetFov() const;



	//=====================================================
	// I_Component
	//=====================================================
	virtual void OnEvent(Core::I_Event& event) override;
	virtual void DebugDrawGUI() override;


	virtual Physics::Primitives::S_Ray GetRay(const glm::vec2& screenPos) const override;

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
	glm::vec3 _center;

	float _fovy;
	float _nearZ;
	float _farZ;
	float _aspect;

	GLE_DEBUG_MEMBER(GUI::Input::C_Slider<float>, float, _zoom);
	GLE_DEBUG_MEMBER(GUI::Input::C_Slider<float>, float, _angleXDeg);
	GLE_DEBUG_MEMBER(GUI::Input::C_Slider<float>, float, _angleYDeg);

	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;

	float m_ControlSpeed;
};
}}