#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Components/ForceLinkComponentTU.h>
#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Resources/RenderResourceHandle.h>

#include <Core/Resources/ResourceHandle.h>

#include <cstdint>
#include <vector>

namespace GLEngine::Renderer {

class C_Material;

struct S_StaticMesh {
	Core::ResourceHandle<MeshResource>					requested;
	Core::ResourceHandle<MeshResource>					live;
	std::vector<Core::ResourceHandle<MaterialResource>> materials;
	std::vector<std::shared_ptr<C_Material>>			convertedMaterials;
	// Snapshot of `materials` as of the last successful conversion into convertedMaterials -
	// lets StaticMeshSystem::UpdateMaterials notice a slot was swapped to a different resource
	// (e.g. via the Editor) even though materials.size() and convertedMaterials didn't change.
	std::vector<Core::ResourceHandle<MaterialResource>> convertedFromMaterials;
};

// GPU-resident per-submesh buffer handles - not RTTR-registered, not routed through
// C_ComponentRegistry: runtime-only, (re)built by StaticMeshSystem, never authored or serialized.
struct S_StaticMeshGPUSubmesh {
	Handle<Buffer> positions;
	Handle<Buffer> normals;
	Handle<Buffer> texCoords;
	Handle<Buffer> tangent;
	Handle<Buffer> bitangent;
	std::uint32_t  numPrimitives = 0;
};

struct S_StaticMeshGPU {
	std::vector<S_StaticMeshGPUSubmesh> submeshes;
	Handle<Pipeline>					pipeline;
};

DECLARE_FORCE_LINK_COMPONENT_TU(EnsureStaticMeshComponentsRegistered);

} // namespace GLEngine::Renderer
