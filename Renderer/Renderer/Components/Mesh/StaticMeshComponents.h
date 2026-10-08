#pragma once

#include <Renderer/RendererApi.h>

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

// See LightComponents.h's EnsureLightComponentsRegistered for why this exists: this TU's only
// content is RTTR_REGISTRATION + a C_ComponentRegistry static-init side effect, so a static-lib
// test link (RendererTest on MSVC) would otherwise drop the whole object file as unused.
RENDERER_API_EXPORT void EnsureStaticMeshComponentsRegistered();

} // namespace GLEngine::Renderer
