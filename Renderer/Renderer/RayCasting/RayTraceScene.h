#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {

class C_RayIntersection;
class I_RayGeometryObject;
class I_RayLight;

class RENDERER_API_EXPORT C_RayTraceScene {
public:
	C_RayTraceScene();
	C_RayTraceScene(const C_RayTraceScene&) = delete;
	~C_RayTraceScene();

	void operator=(const C_RayTraceScene&) = delete;

	[[nodiscard]] bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, float offset = 0.f) const;
	void			   AddObejct(std::unique_ptr<I_RayGeometryObject>&& object);

private:
	std::vector<std::unique_ptr<I_RayGeometryObject>> m_Objects;
};

} // namespace GLEngine::Renderer