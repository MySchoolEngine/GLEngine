#pragma once
#include <Renderer/Colours.h>

namespace GLEngine::Renderer {
class I_Sampler;
class C_RayIntersection;
} // namespace GLEngine::Renderer

namespace GLEngine::Renderer::RayTracing {

struct S_VisibilityTester;
/**
 * http://www.pbr-book.org/3ed-2018/Light_Sources/Light_Interface.html
 */
class I_RayLight {
public:
	virtual ~I_RayLight() = default;
	/**
	 * Samples incident radiance at the intersection given by @param intersection from light.
	 */
	[[nodiscard]] virtual Colours::T_Colour SampleLi(const C_RayIntersection& intersection, I_Sampler* rnd, S_VisibilityTester& vis, float* pdf) const = 0;
	/**
	 * Returns emitted radiance at a surface point intersected by a ray.
	 */
	[[nodiscard]] virtual Colours::T_Colour Le() const = 0;
};
} // namespace GLEngine::Renderer::RayTracing