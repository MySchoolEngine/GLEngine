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
void C_ImageResizer::BilinearDownsampling(const C_TextureView& source, C_TextureView& result)
{
	GLE_ASSERT(source.GetDimensions().x > result.GetDimensions().x, "Source bigger than result");
	GLE_ASSERT(source.GetDimensions().y > result.GetDimensions().y, "Source bigger than result");
	const auto resultDim = result.GetDimensions();
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
} // namespace GLEngine::Renderer
