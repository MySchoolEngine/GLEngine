#pragma once

#include <Renderer/RendererApi.h>

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

#include <rttr/registration_friend.h>
#include <rttr/rttr_enable.h>

namespace GLEngine::Physics::Primitives {
class C_Frustum;
struct S_Ray;
} // namespace GLEngine::Physics::Primitives

namespace GLEngine {
namespace Renderer {

// Kept deriving from Entity::I_Component for now (not decoupled, despite the ECS migration plan's
// original intent) - GLRenderer's ExperimentWindow and VulkanRenderer's VkWindow still attach
// cameras to a player entity through the old Entity::I_Entity::AddComponent/GetComponents(Camera)
// path, which requires this base. Decoupling is deferred to Phase 3/4 when those windows move to
// C_World.
class RENDERER_API_EXPORT I_Camera : public Entity::I_Component {
public:
	explicit I_Camera(const std::shared_ptr<Entity::I_Entity>& owner);
	~I_Camera() override; // = default;
	Entity::E_ComponentType GetType() const override;

	[[nodiscard]] Physics::Primitives::S_AABB GetAABB() const override;

	[[nodiscard]] virtual glm::mat4 GetViewProjectionMatrix() const = 0;
	[[nodiscard]] virtual glm::mat4 GetScreenToWorldMatrix() const	= 0;
	[[nodiscard]] virtual glm::mat4 GetProjectionMatrix() const		= 0;
	[[nodiscard]] virtual glm::mat4 GetViewMatrix() const			= 0;
	[[nodiscard]] virtual glm::quat GetRotation() const				= 0;
	[[nodiscard]] virtual glm::vec3 GetDirection() const			= 0;
	[[nodiscard]] virtual glm::vec3 GetPosition() const				= 0;
	[[nodiscard]] virtual float		GetFar() const					= 0;
	[[nodiscard]] virtual float		GetNear() const					= 0;

	[[nodiscard]] Physics::Primitives::S_Ray GetRay(const glm::vec2& screenPos) const;

	[[nodiscard]] virtual Physics::Primitives::C_Frustum GetFrustum() const = 0;

	// Called by GLEngine::Renderer::CameraSystem::Update for every entity with both
	// S_CameraComponent and S_WorldTransformComponent, right before Update() - lets a concrete
	// camera fold the owning entity's world transform into its own state. Default no-op preserves
	// current behavior for cameras that don't go through C_World yet.
	virtual void SetOwnerWorldTransform(const glm::mat4& worldTransform) {}

	RTTR_ENABLE(Entity::I_Component);
	RTTR_REGISTRATION_FRIEND;
};
} // namespace Renderer

template <> class ComponenetBase<Entity::E_ComponentType::Camera> {
public:
	using type = Renderer::I_Camera;
};
} // namespace GLEngine