#pragma once

namespace GLEngine::Renderer {
class C_SphericalSampler {
public:
	C_SphericalSampler(std::size_t maxSamples);

	[[nodiscard]] glm::vec2 GetDirection(std::size_t sampleNum) const;
private:
	std::size_t m_MaxSamples;
};
}

