#pragma once

#include <Renderer/ICameraComponent.h>

#include <GLRenderer/GUI/Slider.h>

#include <Core/CoreMacros.h>

namespace GLEngine {

namespace Core {
class C_KeyPressedEvent;
class C_MouseScrollEvent;
class C_MouseButtonPressed;
class C_KeyRepeatedEvent;
class C_KeyEvent;
}

namespace Physics {
namespace Primitives {
class C_Frustum;
}
}

namespace GLRenderer {
namespace Cameras {

//Orbital camera
//Using Euclidean angles (= has gimbal lock)
class C_OrbitalCamera : public Renderer::I_CameraComponent
{
public:
	C_OrbitalCamera();
	~C_OrbitalCamera() = default;

	virtual glm::mat4 GetViewProjectionMatrix() const override;
	virtual glm::mat4 GetProjectionMatrix() const override;
	virtual glm::mat4 GetViewMatrix() const override;
	virtual glm::quat GetRotation() const override;
	virtual glm::vec3 GetDirection() const override;
	virtual glm::vec3 GetPosition() const override;

	virtual Physics::Primitives::C_Frustum GetFrustum()	const override;


	virtual float GetAspectRatio() const;

	void setupCameraView(float zoom, glm::vec3 center, float angleXDeg, float angleYDeg);
	void setCenterPoint(const glm::vec3& center);
	void setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg);
	void adjustZoom(int zoom);

	void adjustOrientation(float dx, float dy);

	void DebugDraw();
	virtual void Update() override;


	virtual float GetFar() const { return _farZ; }
	virtual float GetNear() const { return _nearZ; }

	virtual float GetFov() const;



	//=====================================================
	// I_Component
	//=====================================================
	virtual void OnEvent(Core::I_Event& event) override;
	virtual void DebugDrawGUI() override;

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

	GLE_DEBUG_MEMBER(GUI::C_Slider<float>, float, _zoom);
	GLE_DEBUG_MEMBER(GUI::C_Slider<float>, float, _angleXDeg);
	GLE_DEBUG_MEMBER(GUI::C_Slider<float>, float, _angleYDeg);

	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;

	float m_ControlSpeed;
};
}}}