#pragma once

#include <Renderer/RayCasting/Integrator.h>


namespace GLEngine::Renderer {
class C_PathIntegrator : public I_Integrator {
public:
	C_PathIntegrator(const C_RayTraceScene& scene);
	// main API of this class, allows to use of custom sampler
	[[nodiscard]] Colours::T_Colour TraceRay(Physics::Primitives::S_Ray ray, I_Sampler& rnd) override;

private:
	[[nodiscard]] Colours::T_Colour Li_LightSampling(const Physics::Primitives::S_Ray& ray, I_Sampler& rnd, RayTracingSettings::T_ReflAlloc* alloc = nullptr);
	[[nodiscard]] Colours::T_Colour Li_Direct(const Physics::Primitives::S_Ray& ray, I_Sampler& rnd, RayTracingSettings::T_ReflAlloc* alloc = nullptr);
	[[nodiscard]] Colours::T_Colour Li_PathTrace(Physics::Primitives::S_Ray ray, I_Sampler& rnd, RayTracingSettings::T_ReflAlloc* alloc = nullptr);

	[[nodiscard]] Colours::T_Colour
	EstimateDirect(const C_RayIntersection& intersection, const RayTracing::I_RayLight& light, I_Sampler& rnd, RayTracingSettings::T_ReflAlloc* alloc = nullptr);

	const C_RayTraceScene& m_Scene;
};
} // namespace GLEngine::Renderer