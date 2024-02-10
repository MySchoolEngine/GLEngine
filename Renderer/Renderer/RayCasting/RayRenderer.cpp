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
	: m_Scene(scene)
	, m_ProcessedPixels(0)
	, m_MaxDepth(3)
	, m_NewResultAviable(false)
{
}

//=================================================================================
C_RayRenderer::~C_RayRenderer() = default;

//=================================================================================
void C_RayRenderer::Render(I_CameraComponent&	 camera,
						   I_TextureViewStorage& weightedImage,
						   I_TextureViewStorage& storage,
						   std::mutex*			 storageMutex,
						   int					 numSamplesBefore,
						   AdditionalTargets	 additional)
{
	GLE_ASSERT(additional.CheckTargets(storage), "Wrong additional target passed");
	const auto dim	  = storage.GetDimensions();
	m_ProcessedPixels = 0;

	C_PathIntegrator integrator(m_Scene);


	C_STDSampler rnd(0.f, 1.f);

	const auto GetRay = [&](const glm::vec2& screenCoord) {
		const float x = (2.0f * screenCoord.x) / dim.x - 1.0f;
		const float y = 1.0f - (2.0f * screenCoord.y) / dim.y;
		return camera.GetRay({x, y});
	};

	auto textureView  = C_TextureView(&storage);
	auto weightedView = C_TextureView(&weightedImage);
	auto heatMapView  = C_TextureView(additional.rowHeatMap);

	int interleavedLines = 8;

	for (unsigned int y = 0; y < dim.y; y += interleavedLines)
	{
		for (unsigned int x = 0; x < dim.x; ++x)
		{
			::Utils::HighResolutionTimer renderTime;
			const auto				   ray = GetRay(glm::vec2{x, y} + (2.f * rnd.GetV2() - glm::vec2(1.f, 1.f)) / 2.f);
			AddSample({x, y}, textureView, integrator.TraceRay(ray, rnd));
			if (additional.rowHeatMap)
			{
				const auto previousValue = heatMapView.Get<glm::vec4>(glm::ivec2{0, y});
				heatMapView.Set(glm::ivec2{0, y}, previousValue + glm::vec4{renderTime.getElapsedTimeFromLastQueryMilliseconds(), 0, 0, 0.f});
			}
			++m_ProcessedPixels;
		}

		if (storageMutex)
		{
			std::lock_guard<std::mutex> lock(*storageMutex);
			UpdateView(y, interleavedLines, textureView, weightedView, numSamplesBefore);
		}
		else
		{
			UpdateView(y, interleavedLines, textureView, weightedView, numSamplesBefore);
		}
	}

	do
	{
		for (unsigned int y = interleavedLines / 2; y < dim.y; y += interleavedLines)
		{
			for (unsigned int x = 0; x < dim.x; ++x)
			{
				::Utils::HighResolutionTimer renderTime;
				const auto ray = GetRay(glm::vec2{x, y} + (2.f * rnd.GetV2() - glm::vec2(1.f, 1.f)) / 2.f);
				AddSample({x, y}, textureView, integrator.TraceRay(ray, rnd));
				++m_ProcessedPixels;
				if (additional.rowHeatMap) // should be before add sample :( but before TraceRay
				{
					const auto previousValue = heatMapView.Get<glm::vec4>(glm::ivec2{0, y});
					heatMapView.Set(glm::ivec2{0, y}, previousValue + glm::vec4{renderTime.getElapsedTimeFromLastQueryMilliseconds(), 0, 0, 0.f});
				}
			}

			if (storageMutex)
			{
				std::lock_guard<std::mutex> lock(*storageMutex);
				UpdateView(y, interleavedLines / 2, textureView, weightedView, numSamplesBefore);
			}
			else
			{
				UpdateView(y, interleavedLines / 2, textureView, weightedView, numSamplesBefore);
			}
		}
		interleavedLines /= 2;
	} while (interleavedLines > 1);
}

//=================================================================================
void C_RayRenderer::UpdateView(unsigned int sourceLine, unsigned int numLines, C_TextureView& source, C_TextureView& target, unsigned int numSamples)
{
	// basic restriction is that source line has N+1 samples
	// so I need to carry 1/(N+1) part of it to the next lines
	// this holds even for 1st sample on source line
	const auto dim = target.GetDimensions();
	for (unsigned int x = 0; x < dim.x; ++x)
	{
		// sqrt for gamma correction with gamma = 2
		const auto denominator	 = 1.0f / static_cast<float>(numSamples + 1);
		const auto sourceLineVal = source.Get<glm::vec3>(glm::ivec2{x, sourceLine});
		target.Set({x, sourceLine}, glm::sqrt(sourceLineVal * denominator));
		for (unsigned int i = sourceLine + 1; i < std::min(dim.y, sourceLine + numLines); ++i)
		{
			const auto previousLineVal = source.Get<glm::vec3>(glm::ivec2{x, i});
			target.Set({x, i}, glm::sqrt((sourceLineVal * denominator + previousLineVal) * denominator));
		}
	}
	m_NewResultAviable = true;
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
	if (rowHeatMap)
		return rowHeatMap->GetDimensions().x == 1 && rowHeatMap->GetDimensions().y == mainTarget.GetDimensions().y;
	return true;
}

} // namespace GLEngine::Renderer