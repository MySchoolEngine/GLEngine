#pragma once

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>
#include <Entity/IComponent.h>

#include <Physics/Primitives/Frustum.h>

namespace GLEngine {
namespace Renderer {

class I_CameraComponent 
	: public Entity::I_Component{
public:
	virtual ~I_CameraComponent() = default;
	virtual Entity::E_ComponentType GetType() const override
	{
		return Entity::E_ComponentType::Camera;
	}


	virtual glm::mat4 GetViewProjectionMatrix() const = 0;
	virtual glm::mat4 GetProjectionMatrix()		const = 0;
	virtual glm::mat4 GetViewMatrix()			const = 0;
	virtual glm::quat GetRotation()				const = 0;
	virtual glm::vec3 GetDirection()			const = 0;
	virtual glm::vec3 GetPosition()				const = 0;

	virtual Physics::Primitives::C_Frustum GetFrustum()		const = 0;
protected:
	I_CameraComponent() : Entity::I_Component(nullptr) {}
};
}

template<>
class ComponenetBase<Entity::E_ComponentType::Camera> {
public:
	using type = Renderer::I_CameraComponent;
};
}



