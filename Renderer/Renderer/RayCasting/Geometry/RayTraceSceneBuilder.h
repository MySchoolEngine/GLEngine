#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Entity {
class C_EntityManager;
class C_World;
} // namespace GLEngine::Entity

namespace GLEngine::Renderer {
class C_RayTraceScene;
}

namespace GLEngine::Renderer::RayTracing {

/**
 * @brief Collects components of entityManager and converts to ray trace alternatives.
 *
 * @attention Currently only supports static meshes, point lights, area lights
 *
 * @todo Materials from static meshes are only extracted from the original files, not from the scene
 *
 * No longer has a production caller as of Phase 3 Task 3 (RayTrace.cpp's SetScene now takes
 * Entity::C_World& and calls BuildSceneFromWorld below) - kept compiling until Phase 5 deletes the
 * rest of the old ECS, per this migration's convention of not deleting still-compiling old code
 * ahead of that phase.
 **/
RENDERER_API_EXPORT void BuildSceneFromEntityManager(const Entity::C_EntityManager& entityManager, C_RayTraceScene& outScene);

/**
 * @brief Collects components of a C_World and converts them to ray trace alternatives.
 *
 * @attention Currently only supports static meshes, point lights, area lights - sun lights aren't
 * ray-traced and are silently skipped (there's no entt view for them here).
 *
 * @todo Materials from static meshes are only extracted from the original files, not from the scene
 **/
RENDERER_API_EXPORT void BuildSceneFromWorld(Entity::C_World& world, C_RayTraceScene& outScene);

} // namespace GLEngine::Renderer::RayTracing
