#pragma once

#include <Renderer/RayCasting/Light/ILight.h>

namespace GLEngine::Renderer {
class I_Sampler;
} // namespace GLEngine::Renderer

namespace GLEngine::Renderer::RayTracing {
class C_PointLight : public I_RayLight {
public:
	C_PointLight(const glm::vec3& position, const glm::vec3& intenstiy);
	[[nodiscard]] glm::vec3 SampleLi(const C_RayIntersection& intersection, I_Sampler* rnd, S_VisibilityTester& vis, float* pdf) const override;
	[[nodiscard]] glm::vec3 Le() const override;
	[[nodiscard]] float	  PdfLe() const override;

private:
	glm::vec3 m_Position;
	glm::vec3 m_Intensity;
};
} // namespace GLEngine::Renderer::RayTracing