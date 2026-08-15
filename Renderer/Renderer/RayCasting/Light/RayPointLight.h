#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RayCasting/Light/ILight.h>

namespace GLEngine::Renderer {
class I_Sampler;
} // namespace GLEngine::Renderer

namespace GLEngine::Renderer::RayTracing {
class C_PointLight : public I_RayLight {
public:
	C_PointLight(const glm::vec3& position, const glm::vec3& intensity);
	[[nodiscard]] Colours::T_Colour SampleLi(const C_RayIntersection& intersection, I_Sampler& rnd, S_VisibilityTester& vis, float* pdf) const override;
	[[nodiscard]] Colours::T_Colour Le() const override;
	[[nodiscard]] Colours::T_Colour Lo(const glm::vec3& point, const glm::vec3& normal, const glm::vec2& uv, const glm::vec3& w) const override;
	/**
	 * @brief There is no probability of hitting infinitely small point
	 * @param wi ignored
	 * @return
	 */
	[[nodiscard]] float				Pdf_Li(const glm::vec3& wi) const override { return 0.f; }

private:
	Colours::T_Colour m_Position;
	Colours::T_Colour m_Intensity;
};
} // namespace GLEngine::Renderer::RayTracing