#pragma once

#include <Renderer/RayCasting/Light/ILight.h>

namespace GLEngine::Renderer {
class I_Sampler;
} // namespace GLEngine::Renderer

namespace GLEngine::Renderer::RayTracing {
class C_PointLight : public I_RayLight {
public:
	C_PointLight(const glm::vec3& position, const glm::vec3& intenstiy);
	glm::vec3 SampleLi(const C_RayIntersection& intersection, I_Sampler* rnd, glm::vec3& wi, float* distance, float* pdf) const override;
	glm::vec3 Le() const override;
	float	  PdfLe() const override;

private:
	glm::vec3 m_Position;
	glm::vec3 m_Intensity;
};
} // namespace GLEngine::Renderer::RayTracing