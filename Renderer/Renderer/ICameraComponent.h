#pragma once

#include <Renderer/RendererApi.h>

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

#include <rttr/registration_friend.h>
#include <rttr/registration.h>

namespace GLEngine::Physics::Primitives {
class C_Frustum;
struct S_Ray;
} // namespace GLEngine::Physics::Primitives

namespace GLEngine {
namespace Renderer {

class RENDERER_API_EXPORT I_CameraComponent : public Entity::I_Component {
public:
	explicit I_CameraComponent(std::shared_ptr<Entity::I_Entity> owner);
	virtual ~I_CameraComponent(); // = default;
	virtual Entity::E_ComponentType GetType() const override;

	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override;

	[[nodiscard]] virtual glm::mat4 GetViewProjectionMatrix() const = 0;
	[[nodiscard]] virtual glm::mat4 GetScreenToworldMatrix() const	= 0;
	[[nodiscard]] virtual glm::mat4 GetProjectionMatrix() const		= 0;
	[[nodiscard]] virtual glm::mat4 GetViewMatrix() const			= 0;
	[[nodiscard]] virtual glm::quat GetRotation() const				= 0;
	[[nodiscard]] virtual glm::vec3 GetDirection() const			= 0;
	[[nodiscard]] virtual glm::vec3 GetPosition() const				= 0;
	[[nodiscard]] virtual float		GetFar() const					= 0;
	[[nodiscard]] virtual float		GetNear() const					= 0;

	[[nodiscard]] Physics::Primitives::S_Ray GetRay(const glm::vec2& screenPos) const;

	[[nodiscard]] virtual Physics::Primitives::C_Frustum GetFrustum() const = 0;

	RTTR_ENABLE(Entity::I_Component);
	RTTR_REGISTRATION_FRIEND;
};
} // namespace Renderer

template <> class ComponenetBase<Entity::E_ComponentType::Camera> {
public:
	using type = Renderer::I_CameraComponent;
};
} // namespace GLEngine