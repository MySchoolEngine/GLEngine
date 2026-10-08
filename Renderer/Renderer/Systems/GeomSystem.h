#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Entity {
class C_World;
}

namespace GLEngine::Renderer {

class I_Renderer;
class ResourceManager;

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

} // namespace GeomSystem
} // namespace GLEngine::Renderer
