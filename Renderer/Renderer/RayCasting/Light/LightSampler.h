#pragma once

namespace GLEngine::Renderer {
class I_Sampler;
}
namespace GLEngine::Renderer::RayTracing {
class I_RayLight;

class LightSampler {
public:
	virtual ~LightSampler() = default;
	struct LightSample {
		I_RayLight& m_Light;
		float pdf = 0.f;
	};

	virtual std::optional<LightSample> SampleLight(I_Sampler& rnd) const = 0;
};

class UniformLightSample : public LightSampler {
public:
	UniformLightSample(std::span<I_RayLight*> lights);
	std::optional<LightSample> SampleLight(I_Sampler& rnd) const override;

private:
	std::vector<I_RayLight*> m_Lights;
};
} // namespace GLEngine::Renderer::RayTracing