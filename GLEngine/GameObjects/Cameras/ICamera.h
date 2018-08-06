#pragma once

#include <glm/glm.hpp>

namespace Shapes {
class C_Frustum;
}

namespace GameObjects {
namespace Cameras {

class I_Camera {
public:
	virtual ~I_Camera() = default;

	virtual glm::mat4 GetViewProjectionMatrix() const = 0;
	virtual glm::mat4 GetProjectionMatrix()		const = 0;
	virtual glm::mat4 GetViewMatrix()			const = 0;
	virtual glm::quat GetRotation()				const = 0;
	virtual glm::vec3 GetDirection()			const = 0;
	virtual glm::vec3 GetPosition()				const = 0;

	virtual Shapes::C_Frustum GetFrustum()		const = 0;
};

}}