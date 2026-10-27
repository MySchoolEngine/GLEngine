#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Entity {
class C_World;
} // namespace GLEngine::Entity

namespace GLEngine::Renderer {
class C_RayTraceScene;
}

namespace GLEngine::Renderer::RayTracing {

/**
 * @brief Collects components of a C_World and converts them to ray trace alternatives.
 *
 * @attention Currently only supports static meshes, point lights, area lights - sun lights aren't
 * ray-traced and are silently skipped (there's no entt view for them here).
 *
 * @pre Entity::TransformSystem::Update(world) must have run since the last change to any
 * relevant S_TransformComponent - this reads S_WorldTransformComponent as-is and does not
 * recompute it, so a world transform that's stale or still at its default-constructed identity
 * value is read and converted without complaint.
 *
 * @todo Materials from static meshes are only extracted from the original files, not from the scene
 **/
RENDERER_API_EXPORT void BuildSceneFromWorld(const Entity::C_World& world, C_RayTraceScene& outScene);

} // namespace GLEngine::Renderer::RayTracing
