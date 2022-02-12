#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
/**
 * This class will yield uniformly distributed samples for defined
 * @param maxSamples of requested directions. 
 * 
 * If you wish to get directions only sample up until @param maxSamples/2
 * 
 * Based on:
 * https://dl.acm.org/doi/10.1145/2816795.2818131
 */
class RENDERER_API_EXPORT C_SphericalSampler {
public:
	C_SphericalSampler(std::size_t maxSamples);

	[[nodiscard]] glm::vec3 GetDirection(std::size_t sampleNum) const;

private:
	std::size_t m_MaxSamples;
};
} // namespace GLEngine::Renderer
