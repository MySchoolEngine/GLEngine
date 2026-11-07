#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Entity {
class C_World;
}

namespace GLEngine::Renderer::CameraSystem {

// For every entity with both S_CameraComponent and S_WorldTransformComponent: feeds the entity's
// world transform into the camera (I_Camera::SetOwnerWorldTransform) and then calls the camera's
// own Update(). This is what fixes C_OrbitalCamera's historical "ignores the owner matrix" bug -
// only for cameras reached through C_World, not the ones GLRenderer/VulkanRenderer still attach to
// a player entity via the old Entity::I_Entity component system.
RENDERER_API_EXPORT void Update(Entity::C_World& world);

} // namespace GLEngine::Renderer::CameraSystem
