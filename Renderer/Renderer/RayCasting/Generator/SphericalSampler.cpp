#include <RendererStdafx.h>

#include <Renderer/RayCasting/Generator/SphericalSampler.h>

namespace GLEngine::Renderer {

//=================================================================================
C_SphericalSampler::C_SphericalSampler(std::size_t maxSamples)
	: m_MaxSamples(maxSamples)
{
}

//=================================================================================
glm::vec2 C_SphericalSampler::GetDirection(std::size_t sampleNum) const
{
	const auto PHI	= std::sqrt(5.0f) * 0.5f + 0.5f;
	const auto iPHI = static_cast<float>(sampleNum) * (PHI - 1.f);
	const auto phi	= 2.0f * glm::pi<float>() * (iPHI - glm::floor(iPHI));

	const auto cosTheta = 1.0f - (2.0f * sampleNum + 1.0f) * (1.0f / m_MaxSamples);

	const auto sinTheta = std::sqrt(glm::clamp(1.0f - cosTheta * cosTheta, 0.f, 1.f));

	return glm::vec3(glm::cos(phi) * sinTheta, glm::sin(phi) * sinTheta, cosTheta);
}

} // namespace GLEngine::Renderer