#pragma once

#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureResource.h>
#include <Renderer/Textures/TextureView.h>

#include <Core/Resources/LoadingQuery.h>
#include <Core/Resources/ResourceHandle.h>

namespace GLEngine::Physics::Primitives {
struct S_Ray;
}

namespace GLEngine::Renderer {
class C_RayIntersection;
class I_RayGeometryObject;
class TextureResource;
class I_TextureViewStorage;
class I_DebugDraw;
class C_Trimesh;
class I_MaterialInterface;

namespace MeshData {
struct Mesh;
struct Material;
} // namespace MeshData

namespace RayTracing {
class I_RayLight;
class C_AreaLight;
class C_PointLight;
} // namespace RayTracing

class RENDERER_API_EXPORT C_RayTraceScene {
public:
	C_RayTraceScene();
	C_RayTraceScene(const C_RayTraceScene&) = delete;
	~C_RayTraceScene();

	void operator=(const C_RayTraceScene&) = delete;

	[[nodiscard]] bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, float offset = 0.f) const;
	void			   AddObejct(std::shared_ptr<I_RayGeometryObject>&& object);
	void			   AddLight(std::shared_ptr<RayTracing::C_AreaLight>&& light);
	void			   AddLight(std::shared_ptr<RayTracing::C_PointLight>&& light);
	void			   AddMesh(const MeshData::Mesh& mesh, const MeshData::Material& material);

	void ForEachLight(std::function<void(const std::reference_wrapper<const RayTracing::I_RayLight>& light)> fnc) const;

	const C_TextureView GetTextureView(int textureID) const;

	void DebugDraw(I_DebugDraw* dd) const;

	[[nodiscard]] bool IsLoaded() const;
	void			   BuildScene(); //< Call after fully loaded

private:
	std::vector<std::shared_ptr<I_RayGeometryObject>>	   m_Objects;
	std::vector<std::shared_ptr<RayTracing::C_AreaLight>>  m_AreaLights;
	std::vector<std::shared_ptr<RayTracing::C_PointLight>> m_PointLights;
	std::vector<Core::ResourceHandle<TextureResource>>	   m_Textures;
	std::vector<Core::ResourceHandle<MeshResource>>		   m_Meshes;
	std::vector<std::unique_ptr<I_MaterialInterface>>	   m_Materials;
	// aux
	std::vector<std::shared_ptr<C_Trimesh>> m_Trimeshes;

	Core::LoadingQuery m_Loading;

	std::unique_ptr<I_MaterialInterface>& AddMaterial(const MeshData::Material& material);
};

} // namespace GLEngine::Renderer