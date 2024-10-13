#include <RendererStdafx.h>

#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Integrator.h>
#include <Renderer/RayCasting/ProbeRenderer.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {


glm::vec3 SphericalFibbonaciGenerator(const int sample, const int MaxSamples)
{
	const float PHI	 = sqrt(5.0f) * 0.5f + 0.5f;
	const float iPHI = sample * (PHI - 1.f);
	const float phi	 = 2.0f * glm::pi<float>() * (iPHI - floor(iPHI));

	const float cosTheta = 1.0f - (2.0f * sample + 1.0f) * (1.0f / MaxSamples);

	const float sinTheta = sqrt(glm::clamp(1.0f - cosTheta * cosTheta, 0.f, 1.f));

	return glm::vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

//=================================================================================
C_ProbeRenderer::C_ProbeRenderer(const C_RayTraceScene& scene, unsigned int samplesPerRender)
	: m_SamplesPerRender(samplesPerRender)
	, m_Hysteresis(0.f)
	, m_Scene(scene)
	, m_NewResultAviable(false)
{
}

//=================================================================================
C_ProbeRenderer::~C_ProbeRenderer() = default;

//=================================================================================
void C_ProbeRenderer::Render(I_TextureViewStorage& texture, const glm::vec3 probePosition, std::mutex* storageMutex)
{
	struct Result {
		glm::vec3 radiance;
		glm::vec3 direction;
		// float depth; // future work
	};
	std::vector<Result> results;
	results.reserve(m_SamplesPerRender);

	C_PathIntegrator integrator(m_Scene);
	C_STDSampler	 rnd(0.f, 1.f);
	for (unsigned int i = 0; i < m_SamplesPerRender; ++i)
	{
		auto& result	 = results.emplace_back();
		result.direction = SphericalFibbonaciGenerator(i, m_SamplesPerRender);
		result.radiance	 = integrator.TraceRay({probePosition, result.direction}, rnd);
	}
	// integrate
	C_TextureView imageView(&texture);
	const auto	  dim = texture.GetDimensions();
	GLE_ASSERT(dim.x >= 4, "Probe needs to at least 2x2 + border");
	GLE_ASSERT(dim.x == dim.y, "Probe needs to be square");
	const unsigned int probeSideWithBorder = dim.x;
	const unsigned int perProbeResolution  = dim.x - 2;
	for (unsigned int x = 0; x < dim.x; ++x)
	{
		for (unsigned int y = 0; y < dim.y; ++y)
		{
			unsigned int ix = x;
			unsigned int iy = y;

			if (x == 0)
			{
				iy = probeSideWithBorder - y - 2;
			}
			else if (y == 0)
			{
				ix = probeSideWithBorder - x - 2;
			}
			else if (x == probeSideWithBorder - 1)
			{
				ix = perProbeResolution - 1;
				iy = probeSideWithBorder - y - 2;
			}
			else if (y == probeSideWithBorder - 1)
			{
				iy = perProbeResolution - 1;
				ix = probeSideWithBorder - x - 2;
			}
			else
			{
				ix -= 1;
				iy -= 1;
			}
			
			const glm::vec2 coord = (glm::vec2(ix, iy) + glm::vec2(0.5, 0.5)) * (2.0f / float(perProbeResolution)) - glm::vec2(1.0f, 1.0f);

			const auto		pixelDir = oct_to_float32x3(coord);
			glm::vec4		output(0.f);
			for (unsigned int i = 0; i < m_SamplesPerRender; ++i)
			{
				const auto& res	  = results[i];
				const float theta = glm::max(0.0f, glm::dot(res.direction, pixelDir));

				const float weight = theta;
				if (weight >= 1e-4)
					output += glm::vec4(res.radiance * weight, weight);
			}
			const auto		oldValue = imageView.Get<glm::vec3>(glm::uvec2{x, y});
			const glm::vec3 newValue = glm::vec3(output) / output.w;
			imageView.Set({x, y}, m_Hysteresis * oldValue + newValue * (1.f - m_Hysteresis));
		}
	}
	m_NewResultAviable = true;
	m_Hysteresis	   = 0.95f;
}

} // namespace GLEngine::Renderer
