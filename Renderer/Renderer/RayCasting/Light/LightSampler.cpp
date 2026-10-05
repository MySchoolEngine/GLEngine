#include <RendererStdafx.h>

#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Light/LightSampler.h>

namespace GLEngine::Renderer::RayTracing {

//=================================================================================
UniformLightSample::UniformLightSample(std::span<I_RayLight*> lights)
	: m_Lights(lights.begin(), lights.end())
{
}

//=================================================================================
std::optional<LightSampler::LightSample> UniformLightSample::SampleLight(I_Sampler& rnd) const
{
	if (m_Lights.empty())
		return std::nullopt;
	const int lightIndex = std::min<int>(static_cast<int>(rnd.GetD() * m_Lights.size()), static_cast<int>(m_Lights.size() - 1));
	return LightSample{.m_Light = *m_Lights[lightIndex], .pdf = 1.f / m_Lights.size()};
}
} // namespace GLEngine::Renderer::RayTracing