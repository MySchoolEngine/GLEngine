#include <RendererStdAfx.h>

#include <Renderer/RayCasting/Generator/SphericalSampler.h>

namespace GLEngine::Renderer {

//=================================================================================
C_SphericalSampler::C_SphericalSampler(std::size_t maxSamples)
	: m_MaxSamples(maxSamples)
{}

//=================================================================================
glm::vec2 C_SphericalSampler::GetDirection(std::size_t sampleNum) const
{
	const auto PHI = std::sqrt(5.0f) * 0.5f + 0.5f;
	const auto iPHI = static_cast<float>(sampleNum) * (PHI - 1.f);
	const auto phi	= 2.0f * glm::pi<float>() * (iPHI - glm::floor(iPHI));
}


}