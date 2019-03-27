#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera/ICamera.h"


//Orbital camera
//Using Euclidean angles (= has gimbal lock)
class OrbitalCamera : public I_Camera
{
public:
	OrbitalCamera();

	virtual glm::mat4 getViewProjectionMatrix() const override;
	virtual glm::mat4 getViewMatrix() const override;
	virtual glm::mat4 getProjectionMatrix() const override;
    virtual glm::vec3 getPosition() const override;

	void setupCameraView(float zoom, glm::vec3 center,  float angleXDeg, float angleYDeg);
    void setCenterPoint(const glm::vec3& center);
	void setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg);
	void adjustZoom(int zoom);
	
	void adjustOrientation(float dx, float dy);
	
	void update();


	virtual float GetFar() const override { return _farZ; }
	virtual float GetNear() const override { return _nearZ; }

	virtual float GetFov() const override;

	virtual float GetAspectRatio() const override;


	virtual bool Input(SDL_Event) override;


	virtual glm::quat getRotation() const override;

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

	float _zoom;
	float _angleXDeg;
	float _angleYDeg;

	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;
};