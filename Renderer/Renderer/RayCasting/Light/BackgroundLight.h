#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RayCasting/Light/ILight.h>

namespace GLEngine::Renderer::RayTracing {
class C_BackgroundLight : public I_RayLight {
public:
	C_BackgroundLight(const Colours::T_Colour& backgroundColor);
	[[nodiscard]] Colours::T_Colour SampleLi(const C_RayIntersection& intersection, I_Sampler* rnd, S_VisibilityTester& vis, float* pdf) const override;
	[[nodiscard]] Colours::T_Colour Le() const override;

private:
	Colours::T_Colour m_BackgroundColor;
};
} // namespace GLEngine::Renderer::RayTracing
