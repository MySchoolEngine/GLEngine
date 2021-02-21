#pragma once

#include <glm/glm.hpp>

namespace GLEngine::Renderer {
class I_Sampler;
class C_RayIntersection;
} // namespace GLEngine::Renderer

namespace GLEngine::Renderer::RayTracing {

class I_RayLight {
public:
	virtual ~I_RayLight()																															  = default;
	[[nodiscard]] virtual glm::vec3 SampleLi(const C_RayIntersection& intersection, I_Sampler* rnd, glm::vec3& wi, float* distance, float* pdf) const = 0;
	[[nodiscard]] virtual glm::vec3 Le() const																										  = 0;
	[[nodiscard]] virtual float		PdfLe() const																									  = 0;
};
} // namespace GLEngine::Renderer::RayTracing