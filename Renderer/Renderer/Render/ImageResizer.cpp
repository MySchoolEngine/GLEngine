#include <RendererStdafx.h>

#include <Renderer/Render/ImageResizer.h>


namespace GLEngine::Renderer {
//=================================================================================
void C_ImageResizer::SincDownsampling(const C_TextureView& source, C_TextureView& result)
{
	GLE_ASSERT(source.GetDimensions().x > result.GetDimensions().x, "Source bigger than result");
	GLE_ASSERT(source.GetDimensions().y > result.GetDimensions().y, "Source bigger than result");
}

//=================================================================================
void C_ImageResizer::BilinearDownsampling(const C_TextureView& source, C_TextureView& result, const bool bKeepRatio)
{
	GLE_ASSERT(source.GetDimensions().x > result.GetDimensions().x, "Source bigger than result");
	GLE_ASSERT(source.GetDimensions().y > result.GetDimensions().y, "Source bigger than result");
	const auto resultDim = result.GetDimensions();
	if (!bKeepRatio)
	{
		for (unsigned int x = 0; x < resultDim.x; ++x)
		{
			for (unsigned int y = 0; y < resultDim.y; ++y)
			{
				const glm::vec2 uv	   = result.GetUVForPixel({x, y});
				auto			sample = source.Get<glm::vec4, T_Bilinear>(uv);
				result.Set<glm::vec4>({x, y}, std::move(sample));
			}
		}
	}
	else
	{
		// output_start
		// 	+ ((output_end - output_start) / (input_end - input_start))
		// 		  * (input - input_start)

		auto map = [](unsigned int value, unsigned int minIn, unsigned int minOut, unsigned int maxIn, unsigned int maxOut) -> unsigned int {
			return minOut + ((maxOut - minOut) / (float)(maxIn - minIn)) * (value - minIn);
		};
		const auto	 sourceDim = source.GetDimensions();
		unsigned int XOffset   = 0;
		unsigned int YOffset   = 0;
		// bigger side will take whole space, smaller will get into the center
		if (sourceDim.x > sourceDim.y)
		{
			YOffset = resultDim.y - static_cast<unsigned int>(static_cast<float>(sourceDim.y) / static_cast<float>(sourceDim.x) * static_cast<float>(resultDim.y));
		}
		else
		{
			XOffset = resultDim.x - static_cast<unsigned int>(static_cast<float>(sourceDim.x) / static_cast<float>(sourceDim.y) * static_cast<float>(resultDim.x));
		}
		for (unsigned int x = 0; x < resultDim.x; ++x)
		{
			for (unsigned int y = 0; y < resultDim.y; ++y)
			{
				if (y < YOffset / 2)
					continue;
				if (y > (resultDim.y - (YOffset / 2)))
					continue;
				if (x < XOffset / 2)
					continue;
				if (x > (resultDim.x - (XOffset / 2)))
					break;

				const glm::vec2 uv = result.GetUVForPixel(
					{
						map(x, (XOffset / 2), 0, resultDim.x - (XOffset / 2), resultDim.x - 1),
						map(y, (YOffset / 2), 0, resultDim.y - (YOffset / 2), resultDim.y - 1)
					});
				auto sample = source.Get<glm::vec4, T_Bilinear>(uv);
				result.Set<glm::vec4>({x, y}, std::move(sample));
			}
		}
	}
}
} // namespace GLEngine::Renderer
