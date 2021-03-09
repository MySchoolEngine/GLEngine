#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Physics::Primitives {
struct S_Ray;
}

namespace GLEngine::Renderer {
class C_RayIntersection;
class I_RayGeometryObject;

namespace MeshData {
struct Mesh;
}

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
	void			   AddMesh(const MeshData::Mesh& mesh);

	void ForEachLight(std::function<void(const std::reference_wrapper<const RayTracing::I_RayLight>& light)> fnc) const;

private:
	std::vector<std::shared_ptr<I_RayGeometryObject>>	   m_Objects;
	std::vector<std::shared_ptr<RayTracing::C_AreaLight>>  m_AreaLights;
	std::vector<std::shared_ptr<RayTracing::C_PointLight>> m_PointLights;
};

} // namespace GLEngine::Renderer