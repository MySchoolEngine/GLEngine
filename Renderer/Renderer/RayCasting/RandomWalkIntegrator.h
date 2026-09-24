#pragma once

#include <Renderer/RayCasting/Integrator.h>

namespace GLEngine::Renderer {
class RandomWalkIntegrator : public I_Integrator {
public:
	struct IntegratorSettings {
		const C_RayTraceScene& Scene;
		unsigned int		   MaxDepth;
	};
	RandomWalkIntegrator(const IntegratorSettings& settings);
	// main API of this class, allows to use of custom sampler
	[[nodiscard]] Colours::T_Colour TraceRay(Physics::Primitives::S_Ray ray, I_Sampler& rnd) override;

private:
	[[nodiscard]] Colours::T_Colour LiRandomWalk(Physics::Primitives::S_Ray ray, I_Sampler& rnd, unsigned int depth, RayTracingSettings::T_ReflAlloc* alloc);

	unsigned int m_MaxDepth = 3;
};
} // namespace GLEngine::Renderer