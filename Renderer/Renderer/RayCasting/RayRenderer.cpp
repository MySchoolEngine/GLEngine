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
	, m_NewResultAvailable(false)
	, m_Scene(scene)
{
}

//=================================================================================
C_RayRenderer::~C_RayRenderer() = default;

//=================================================================================
void C_RayRenderer::Render(const RenderSettings& settings, I_TextureViewStorage& weightedImage, I_TextureViewStorage& storage, std::mutex* storageMutex)
{
	GLE_ASSERT(settings.additional.CheckTargets(storage), "Wrong additional target passed");
	const auto dim	  = storage.GetDimensions();
	m_ProcessedPixels = 0;

	C_STDSampler	 rnd(0.f, 1.f);

	const auto GetRay = [&](const glm::vec2& screenCoord) {
		const float x = (2.0f * screenCoord.x) / dim.x - 1.0f;
		const float y = 1.0f - (2.0f * screenCoord.y) / dim.y;
		return settings.camera.GetRay({x, y});
	};

	auto textureView  = C_TextureView(&storage);
	auto weightedView = C_TextureView(&weightedImage);
	auto heatMapView  = C_TextureView(settings.additional.rowHeatMap);

	for (const auto& unit : settings.generatorFactory(dim))
	{
		for (unsigned int y = unit.renderMin.y; y < unit.renderMax.y; ++y)
		{
			for (unsigned int x = unit.renderMin.x; x < unit.renderMax.x; ++x)
			{
				::Utils::HighResolutionTimer renderTime;
				const auto					 ray = GetRay(glm::vec2{x, y} + (2.f * rnd.GetV2() - glm::vec2(1.f, 1.f)) / 2.f);
				{
					GL_PROFILE_SCOPE_N("TraceRay");
					AddSample({x, y}, textureView, settings.integrator->TraceRay(ray, rnd));
				}
				++m_ProcessedPixels;
				if (settings.additional.rowHeatMap) // should be before add sample :( but before TraceRay
				{
					const auto previousValue = heatMapView.Get<glm::vec3>(glm::ivec2{0, y});
					heatMapView.Set({0, y}, previousValue + glm::vec3{renderTime.getElapsedTimeFromLastQueryMilliseconds(), 0, 0});
				}
				// todo move all this to separate functionality
				if (settings.numSamplesBefore == 0)
				{
					C_RayIntersection intersection;
					bool			  hit = false;
					if (settings.additional.normalsMap || settings.additional.uvMap)
					{
						hit = m_Scene.Intersect(ray, intersection);
					}
					if (hit)
					{
						if (settings.additional.normalsMap)
						{
							auto normalView = C_TextureView(settings.additional.normalsMap);
							normalView.Set({x, y}, intersection.GetFrame().Normal());
						}
						if (settings.additional.uvMap)
						{
							auto uvView = C_TextureView(settings.additional.uvMap);
							uvView.Set({x, y}, glm::vec3{intersection.GetUV(), 0.f});
						}
					}
				}
			}
		}

		if (storageMutex)
		{
			GL_PROFILE_SCOPE_N("Wait for mutex");
			std::lock_guard<std::mutex> lock(*storageMutex);
			UpdateView(unit, textureView, weightedView, settings.numSamplesBefore);
		}
		else
		{
			UpdateView(unit, textureView, weightedView, settings.numSamplesBefore);
		}
	}
}

//=================================================================================
void C_RayRenderer::UpdateView(const S_RenderWorkUnit& unit, const C_TextureView& source, C_TextureView& target, const unsigned int numSamples)
{
	GL_PROFILE_SCOPE_N("UpdateView");
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
void C_RayRenderer::AddSample(const glm::ivec2 coord, C_TextureView& view, const glm::vec3 sample)
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
		ok &= uvMap->GetDimensions() == mainTarget.GetDimensions();
	return ok;
}

} // namespace GLEngine::Renderer