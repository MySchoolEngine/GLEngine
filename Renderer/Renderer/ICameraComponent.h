#pragma once

#include <Renderer/RendererApi.h>

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

#include <Physics/Primitives/Frustum.h>

namespace GLEngine {
namespace Renderer {

class RENDERER_API_EXPORT I_CameraComponent : public Entity::I_Component {
public:
  explicit I_CameraComponent(std::shared_ptr<Entity::I_Entity> &owner);
  virtual ~I_CameraComponent(); // = default;
  virtual Entity::E_ComponentType GetType() const override;

  virtual Physics::Primitives::S_AABB GetAABB() const override;

  virtual glm::mat4 GetViewProjectionMatrix() const = 0;
  virtual glm::mat4 GetScreenToworldMatrix() const = 0;
  virtual glm::mat4 GetProjectionMatrix() const = 0;
  virtual glm::mat4 GetViewMatrix() const = 0;
  virtual glm::quat GetRotation() const = 0;
  virtual glm::vec3 GetDirection() const = 0;
  virtual glm::vec3 GetPosition() const = 0;

  Physics::Primitives::S_Ray GetRay(const glm::vec2 &screenPos) const;

  virtual Physics::Primitives::C_Frustum GetFrustum() const = 0;
};
} // namespace Renderer

template <> class ComponenetBase<Entity::E_ComponentType::Camera> {
public:
  using type = Renderer::I_CameraComponent;
};
} // namespace GLEngine