#include <RendererStdafx.h>

#include <Renderer/RayCasting/RayGeneration/InterleavedLinesFactory.h>

namespace GLEngine::Renderer {

//=================================================================================
Generator<S_RenderWorkUnit> C_InterleavedLinesFactory::operator()(glm::uvec2 imageDim) const
{
	const unsigned int width  = imageDim.x;
	const unsigned int height = imageDim.y;

	unsigned int interleavedLines = factor;
	unsigned int currentStartLine = 0;
	unsigned int numViewLines	  = factor;
	unsigned int toNextLine		  = factor;

	do
	{
		for (unsigned int y = currentStartLine; y < height; y += toNextLine)
		{
			co_yield S_RenderWorkUnit{
				.renderMin = {0u, y},
				.renderMax = {width, y + 1u},
				.viewMin   = {0u, y},
				.viewMax   = {width, std::min(y + numViewLines, height)},
			};
		}

		if (interleavedLines == 1)
			break;

		toNextLine = interleavedLines;
		interleavedLines /= 2;
		currentStartLine = interleavedLines;
		numViewLines	 = interleavedLines;
	} while (true);
}

} // namespace GLEngine::Renderer
