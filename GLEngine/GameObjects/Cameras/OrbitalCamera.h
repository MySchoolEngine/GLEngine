#pragma once

#include "GameObjects/Cameras/ICamera.h"

namespace GameObjects {
namespace Cameras {

class C_OrbitalCamera 
: public I_Camera{
public:
	C_OrbitalCamera(glm::vec3 center);
	virtual ~C_OrbitalCamera() = default;

	virtual glm::mat4 GetViewProjectionMatrix() const override;
	virtual glm::mat4 GetProjectionMatrix()		const override;
	virtual glm::mat4 GetViewMatrix()			const override;
	virtual glm::quat GetRotation()				const override;
	virtual glm::vec3 GetDirection()			const override;
	virtual glm::vec3 GetPosition()				const override;
	virtual Shapes::C_Frustum GetFrustum()		const override;


	glm::vec3	GetCenter() const { return m_Center; }
	void		SetCenter(const glm::vec3 val) { m_Center = val; }

	void setupCameraView(float zoom, glm::vec3 center, float angleXDeg, float angleYDeg);
	void setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg);
private:
	void Update();

	glm::vec3 m_Center;
	glm::vec3 m_Pos;
	glm::vec3 m_View;
	glm::vec3 m_Left;
	glm::vec3 m_Up;
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;

	float m_FovY;
	float m_NearZ;
	float m_FarZ;
	float m_Aspect;

	float m_Zoom;
	float m_AngleXDeg;
	float m_AngleYDeg;

};
}
}

