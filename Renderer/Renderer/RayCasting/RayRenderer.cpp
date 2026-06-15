#include <RendererStdafx.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Integrator.h>
#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/Textures/TextureView.h>

#include <Utils/HighResolutionTimer.h>

namespace GLEngine::Renderer {

//=================================================================================
C_RayRenderer::C_RayRenderer(const C_RayTraceScene& scene)
	: m_ProcessedPixels(0)
	, m_MaxDepth(3)
	, m_NewResultAvailable(false)
	, m_Scene(scene)
{
}

//=================================================================================
C_RayRenderer::~C_RayRenderer() = default;

//=================================================================================
void C_RayRenderer::Render(I_CameraComponent&	  camera,
						   I_TextureViewStorage&  weightedImage,
						   I_TextureViewStorage&  storage,
						   std::mutex*			  storageMutex,
						   int					  numSamplesBefore,
						   std::function<Generator<S_RenderWorkUnit>(glm::uvec2)> generatorFactory,
						   AdditionalTargets	  additional)
{
	GLE_ASSERT(additional.CheckTargets(storage), "Wrong additional target passed");
	const auto dim	  = storage.GetDimensions();
	m_ProcessedPixels = 0;

	C_PathIntegrator integrator(m_Scene);
	C_STDSampler	 rnd(0.f, 1.f);

	const auto GetRay = [&](const glm::vec2& screenCoord) {
		const float x = (2.0f * screenCoord.x) / dim.x - 1.0f;
		const float y = 1.0f - (2.0f * screenCoord.y) / dim.y;
		return camera.GetRay({x, y});
	};

	auto textureView  = C_TextureView(&storage);
	auto weightedView = C_TextureView(&weightedImage);
	auto heatMapView  = C_TextureView(additional.rowHeatMap);

	for (const auto& unit : generatorFactory(dim))
	{
		for (unsigned int y = unit.renderMin.y; y < unit.renderMax.y; ++y)
		{
			for (unsigned int x = unit.renderMin.x; x < unit.renderMax.x; ++x)
			{
				::Utils::HighResolutionTimer renderTime;
				const auto				   ray = GetRay(glm::vec2{x, y} + (2.f * rnd.GetV2() - glm::vec2(1.f, 1.f)) / 2.f);
				AddSample({x, y}, textureView, integrator.TraceRay(ray, rnd));
				++m_ProcessedPixels;
				if (additional.rowHeatMap) // should be before add sample :( but before TraceRay
				{
					const auto previousValue = heatMapView.Get<glm::vec3>(glm::ivec2{0, y});
					heatMapView.Set({0, y}, previousValue + glm::vec3{renderTime.getElapsedTimeFromLastQueryMilliseconds(), 0, 0});
				}
				if (numSamplesBefore == 0)
				{
					C_RayIntersection intersection;
					bool			  hit = false;
					if (additional.normalsMap || additional.uvMap)
					{
						hit = m_Scene.Intersect(ray, intersection);
					}
					if (hit)
					{
						if (additional.normalsMap)
						{
							auto normalView = C_TextureView(additional.normalsMap);
							normalView.Set({x, y}, intersection.GetFrame().Normal());
						}
						if (additional.uvMap)
						{
							auto uvView = C_TextureView(additional.uvMap);
							uvView.Set({x, y}, glm::vec3{intersection.GetUV(), 0.f});
						}
					}
				}
			}
		}

		if (storageMutex)
		{
			std::lock_guard<std::mutex> lock(*storageMutex);
			UpdateView(unit, textureView, weightedView, numSamplesBefore);
		}
		else
		{
			UpdateView(unit, textureView, weightedView, numSamplesBefore);
		}
	}
}

//=================================================================================
void C_RayRenderer::UpdateView(const S_RenderWorkUnit& unit, const C_TextureView& source, C_TextureView& target, const unsigned int numSamples)
{
	const auto denominator = 1.0f / static_cast<float>(numSamples + 1);

	for (unsigned int x = unit.renderMin.x; x < unit.renderMax.x; ++x)
	{
		// sqrt for gamma correction with gamma = 2
		for (unsigned int y = unit.renderMin.y; y < unit.renderMax.y; ++y)
		{
			const auto val = source.Get<glm::vec3>({x, y});
			target.Set({x, y}, glm::sqrt(val * denominator));
		}

		// Fill-in preview rows beyond the rendered region (interleaved pattern only).
		// Uses the last rendered row blended with what was previously in source.
		const auto lastRenderedRow = unit.renderMax.y - 1;
		const auto sourceLineVal   = source.Get<glm::vec3>({x, lastRenderedRow});
		for (unsigned int i = unit.renderMax.y; i < unit.viewMax.y; ++i)
		{
			const auto previousLineVal = source.Get<glm::vec3>({x, i});
			target.Set({x, i}, glm::sqrt((sourceLineVal * denominator + previousLineVal) * denominator));
		}
	}
	m_NewResultAvailable = true;
}

//=================================================================================
void C_RayRenderer::AddSample(const glm::ivec2 coord, C_TextureView view, const glm::vec3 sample)
{
	const auto previousValue = view.Get<glm::vec4>(coord);
	view.Set(coord, previousValue + glm::vec4(sample, 0.f));
}
//=================================================================================
std::size_t C_RayRenderer::GetProcessedPixels() const
{
	return m_ProcessedPixels;
}

//=================================================================================
bool C_RayRenderer::AdditionalTargets::CheckTargets(const I_TextureViewStorage& mainTarget) const
{
	bool ok = true;
	if (rowHeatMap)
		ok &= rowHeatMap->GetDimensions().x == 1 && rowHeatMap->GetDimensions().y == mainTarget.GetDimensions().y;
	if (normalsMap)
		ok &= normalsMap->GetDimensions() == mainTarget.GetDimensions();
	if (uvMap)
		ok &= normalsMap->GetDimensions() == mainTarget.GetDimensions();
	return ok;
}

} // namespace GLEngine::Renderer