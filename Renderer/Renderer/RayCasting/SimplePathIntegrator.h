#pragma once

#include <Renderer/RayCasting/Integrator.h>
#include <Renderer/RayCasting/Light/LightSampler.h>

namespace GLEngine::Renderer::RayTracing {

class SimplePathIntegrator : public I_Integrator {
public:
	struct IntegratorSettings {
		const C_RayTraceScene& Scene;
		unsigned int		   MaxDepth;
		bool				   SampleLights;
		bool				   SampleBRDF;
	};
	SimplePathIntegrator(const IntegratorSettings& settings);

	[[nodiscard]] Colours::T_Colour TraceRay(Physics::Primitives::S_Ray ray, I_Sampler& rnd) override;

private:
	unsigned int  m_MaxDepth = 3;
	bool		  m_SampleLights;
	bool		  m_SampleBRDF;
	std::unique_ptr<LightSampler> m_LightSampler;
};
} // namespace GLEngine::Renderer::RayTracing
