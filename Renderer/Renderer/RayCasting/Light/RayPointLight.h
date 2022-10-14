#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RayCasting/Light/ILight.h>

namespace GLEngine::Renderer {
class I_Sampler;
} // namespace GLEngine::Renderer

namespace GLEngine::Renderer::RayTracing {
class C_PointLight : public I_RayLight {
public:
	C_PointLight(const glm::vec3& position, const glm::vec3& intenstiy);
	[[nodiscard]] Colours::T_Colour SampleLi(const C_RayIntersection& intersection, I_Sampler* rnd, S_VisibilityTester& vis, float* pdf) const override;
	[[nodiscard]] Colours::T_Colour Le() const override;

private:
	Colours::T_Colour m_Position;
	Colours::T_Colour m_Intensity;
};
} // namespace GLEngine::Renderer::RayTracing