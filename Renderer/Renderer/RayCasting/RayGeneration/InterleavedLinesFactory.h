#pragma once

#include <Renderer/RayCasting/RayGeneration/Generator.h>
#include <Renderer/RayCasting/RayGeneration/RenderWorkUnit.h>
#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {

// Callable factory: operator() is a coroutine that co_yields one S_RenderWorkUnit
// per rendered line, across multiple passes with a halving stride.
//
//   factor=4: pass1 lines 0,4,8,...  (viewLines=4)
//             pass2 lines 2,6,10,... (viewLines=2)
//             pass3 lines 1,3,5,...  (viewLines=1)
struct RENDERER_API_EXPORT C_InterleavedLinesFactory {
	unsigned int factor = 4;

	Generator<S_RenderWorkUnit> operator()(glm::uvec2 imageDim) const;
};

} // namespace GLEngine::Renderer
