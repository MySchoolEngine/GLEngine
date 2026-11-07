#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Entity {
class C_World;
struct S_WorldTransformComponent;
} // namespace GLEngine::Entity

namespace GLEngine::Renderer {

class I_Renderer;
class ResourceManager;
class Renderer3D;
struct S_StaticMesh;
struct S_StaticMeshGPU;

namespace StaticMeshSystem {

// (Re)builds S_StaticMeshGPU for every S_StaticMesh entity whose requested resource became ready
// and differs from what's already live. Takes the renderer explicitly (not via a global
// Core::C_Application::Get() singleton) so this is callable from a unit test against a fake
// I_Renderer/ResourceManager, with no real GL context.
RENDERER_API_EXPORT void Update(Entity::C_World& world, I_Renderer& renderer);

// Submits one RenderCall3D per submesh - ported from C_StaticMeshHandles::Render. Callers (e.g.
// MainPassTechnique, SunShadowMapTechnique) iterate view<S_StaticMesh, S_StaticMeshGPU,
// S_WorldTransformComponent>() and call this per matching entity.
RENDERER_API_EXPORT void Render(const S_StaticMesh& mesh, const S_StaticMeshGPU& gpu, const Entity::S_WorldTransformComponent& worldTransform, Renderer3D& renderer3D);

// Connects the on_destroy<S_StaticMeshGPU> hook that frees GPU buffers - fixes the leak where
// C_StaticMeshHandles's destructor never called CleanRenderData. Call exactly once per world's
// registry lifetime (e.g. at world/renderer setup), not per-frame - EnTT does not deduplicate
// repeated connect() calls to the same handler.
RENDERER_API_EXPORT void RegisterDestroyHook(Entity::C_World& world, ResourceManager& resourceManager);

} // namespace StaticMeshSystem
} // namespace GLEngine::Renderer
