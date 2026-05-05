#pragma once

#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/RendererApi.h>

#include <Core/Resources/ResourceHandle.h>

#include <filesystem>
#include <vector>

namespace GLEngine::Renderer {
class MeshResource;
class MaterialResource;

// Extracts one MaterialResource per material in the MeshResource.
// Output files named: <meshStem>-<sanitisedMaterialName>.glmat placed in outputDir.
// Returns ResourceHandle to each created MaterialResource (loaded and tracked by ResourceManager).
//
// For single-material meshes prefer rm.LoadResource<MaterialResource>(meshPath) instead
// — it auto-builds via the derived resource system.
RENDERER_API_EXPORT std::vector<Core::ResourceHandle<MaterialResource>> ExtractMaterialsFromMesh(const MeshResource& mesh, const std::filesystem::path& outputDir);

} // namespace GLEngine::Renderer
