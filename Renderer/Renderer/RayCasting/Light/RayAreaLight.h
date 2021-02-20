#pragma once

#include <Renderer/RayCasting/Light/ILight.h>

namespace GLEngine::Renderer {
class I_RayGeometryObject;
class I_Sampler;
} // namespace GLEngine::Renderer

namespace GLEngine::Renderer::RayTracing {
class C_AreaLight : public I_RayLight {
public:
	C_AreaLight(const glm::vec3& radiance, const std::shared_ptr<I_RayGeometryObject>& shape);
	[[nodiscard]] virtual glm::vec3 SampleLi() const override;
	[[nodiscard]] virtual glm::vec3 Le() const override;
	[[nodiscard]] virtual float		PdfLe() const override;

	[[nodiscard]] std::shared_ptr<I_RayGeometryObject> GetGeometry() const;

private:
	glm::vec3							 m_Radiance;
	std::shared_ptr<I_RayGeometryObject> m_Shape;
};
} // namespace GLEngine::Renderer::RayTracing
