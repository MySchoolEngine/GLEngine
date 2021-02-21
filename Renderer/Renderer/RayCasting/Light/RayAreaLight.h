#pragma once

#include <Renderer/RayCasting/Light/ILight.h>
#include <Renderer/RayCasting/SceneGeometry.h>

namespace GLEngine::Physics::Primitives {
struct S_Disc;
} // namespace GLEngine::Physics::Primitives

namespace GLEngine::Renderer {
class I_RayGeometryObject;
class I_Sampler;
} // namespace GLEngine::Renderer

namespace GLEngine::Renderer::RayTracing {
class C_AreaLight : public I_RayLight {
public:
	C_AreaLight(const glm::vec3& radiance, const std::shared_ptr<C_Primitive<Physics::Primitives::S_Disc>>& shape);
	[[nodiscard]] virtual glm::vec3 SampleLi(const C_RayIntersection& intersection, I_Sampler* rnd, glm::vec3& wi, float* distance, float* pdf) const override;
	[[nodiscard]] virtual glm::vec3 Le() const override;
	[[nodiscard]] virtual float		PdfLe() const override;

	[[nodiscard]] std::shared_ptr<I_RayGeometryObject> GetGeometry() const;

private:
	glm::vec3												  m_Radiance;
	std::shared_ptr<C_Primitive<Physics::Primitives::S_Disc>> m_Shape;
};
} // namespace GLEngine::Renderer::RayTracing
