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
struct S_GeomComponent;
struct S_GeomGPU;

namespace GeomSystem {

// Builds S_GeomGPU (the hardcoded procedural plane) for every S_GeomComponent entity that
// doesn't have one yet - unlike StaticMeshSystem there is no "resource changed, rebuild" case,
// since the old C_GeomComponent never loads from a file; it always builds the same plane once,
// in its constructor. Also assigns a default material the first time, mirroring the old
// constructor's C_MaterialManager::GetMaterial("GeomComponent")-or-register fallback.
RENDERER_API_EXPORT void Update(Entity::C_World& world, I_Renderer& renderer);

// Connects the on_destroy<S_GeomGPU> hook that frees GPU buffers - call exactly once per world's
// registry lifetime, not per-frame (see StaticMeshSystem::RegisterDestroyHook for why).
RENDERER_API_EXPORT void RegisterDestroyHook(Entity::C_World& world, ResourceManager& resourceManager);

// Submits a RenderCall3D for the one procedural mesh - ported from C_GeomComponent::Render.
RENDERER_API_EXPORT void Render(const S_GeomComponent& geom, const S_GeomGPU& gpu, const Entity::S_WorldTransformComponent& worldTransform, Renderer3D& renderer3D);

} // namespace GeomSystem
} // namespace GLEngine::Renderer
