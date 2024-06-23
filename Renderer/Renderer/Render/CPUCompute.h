#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {
class RENDERER_API_EXPORT C_CPUCompute {
public:
	C_CPUCompute();
	void ComputeHistogram(const C_TextureView view, C_TextureView target);
};
} // namespace GLEngine::Renderer
