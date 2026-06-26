#pragma once

namespace GLEngine::Renderer {

// One unit of work yielded by a ray generation factory.
//
// renderMin/renderMax  pixel region to trace rays for (renderMax is exclusive).
// viewMin/viewMax      region to update in the weighted view after tracing (exclusive max).
//   Interleaved: viewMax.y extends below renderMax.y for the rough preview fill-in.
//   Tiled/scanline:    viewMax == renderMax (copy exactly, no fill-in).
struct S_RenderWorkUnit {
	glm::uvec2 renderMin;
	glm::uvec2 renderMax;
	glm::uvec2 viewMin;
	glm::uvec2 viewMax;
};

} // namespace GLEngine::Renderer
