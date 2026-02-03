#include <RendererStdafx.h>

#include <Renderer/Render/CPUCompute.h>

#include <Utils/HighResolutionTimer.h>

namespace GLEngine::Renderer {

//=================================================================================
C_CPUCompute::C_CPUCompute()
{
}

//=================================================================================
void C_CPUCompute::ComputeHistogram(const C_TextureView view, C_TextureView target)
{
	Utils::HighResolutionTimer computeHistogramTimer;
	GLE_ASSERT(target.GetDimensions().y == 1, "Wrong target for histogram.");
	target.ClearColor({Colours::black, 0.f});
	const float bucketSize = 1.f / target.GetDimensions().x;
	if (target.GetStorage()->GetNumElements() == 1)
	{
		for (unsigned int x = 0; x < view.GetDimensions().x; ++x)
		{
			for (unsigned int y = 0; y < view.GetDimensions().y; ++y) {
				const glm::vec3 val = view.Get<glm::vec3>(glm::uvec2{x, y});
				const auto		valLuminance = Colours::ToLuminance(val);
				int				bucket		 = valLuminance / bucketSize;
				target.Set(glm::ivec2{bucket, 0}, target.Get<float>(glm::uvec2{bucket, 0}, E_TextureChannel::Red) + 1.f, E_TextureChannel::Red);
			}
		}
	}


	CORE_LOG(E_Level::Info, E_Context::Render, "Histogram generation time {}s", static_cast<float>(computeHistogramTimer.getElapsedTimeFromLastQueryMilliseconds()) / 1000.f);
}

} // namespace GLEngine::Renderer