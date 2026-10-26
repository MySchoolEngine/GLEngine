#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Resources/RenderResourceHandle.h>

#include <cstdint>
#include <memory>

namespace GLEngine::Renderer {

class C_Material;

// Unlike S_StaticMesh, this has no resource handle at all - the old C_GeomComponent always builds
// a hardcoded procedural plane (MeshData::C_Geometry::CreatePlane()), never loads from a file.
struct S_GeomComponent {
	std::shared_ptr<C_Material> material;
};

// GPU-resident buffer handles for the one procedural mesh - not RTTR-registered, not routed
// through C_ComponentRegistry: runtime-only, built once by GeomSystem. Unlike S_StaticMeshGPU,
// there is only ever one mesh here (no per-submesh vector), matching C_GeomComponent's single
// MeshContainer.
struct S_GeomGPU {
	Handle<Buffer>	 positions;
	Handle<Buffer>	 normals;
	Handle<Buffer>	 texCoords;
	Handle<Buffer>	 tangent;
	Handle<Buffer>	 bitangent;
	std::uint32_t	 numPrimitives = 0;
	Handle<Pipeline> pipeline;
};

// See LightComponents.h's EnsureLightComponentsRegistered for why this exists (force-link for
// static-lib test builds).
RENDERER_API_EXPORT void EnsureGeomComponentsRegistered();

} // namespace GLEngine::Renderer
