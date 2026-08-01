#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Entity {
class C_EntityManager;
}

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
 **/
RENDERER_API_EXPORT void BuildSceneFromEntityManager(const Entity::C_EntityManager& entityManager, C_RayTraceScene& outScene);

} // namespace GLEngine::Renderer::RayTracing
