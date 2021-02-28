#pragma once

namespace GLEngine::Renderer {
class I_Sampler {
public:
	virtual ~I_Sampler()					= default;
	[[nodiscard]] virtual float		GetD()	= 0;
	[[nodiscard]] virtual glm::vec2 GetV2() = 0;
	[[nodiscard]] virtual glm::vec3 GetV3() = 0;
};

class C_STDSampler : public I_Sampler {
public:
	C_STDSampler(float min, float max);
	[[nodiscard]] virtual float		GetD() override;
	[[nodiscard]] virtual glm::vec2 GetV2() override;
	[[nodiscard]] virtual glm::vec3 GetV3() override;

private:
	std::mt19937						  m_Gen;
	std::uniform_real_distribution<float> m_Distrib;
};

} // namespace GLEngine::Renderer