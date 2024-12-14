#pragma once

#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {
class RENDERER_API_EXPORT C_ImageResizer {
public:

	static void SincDownsampling(const C_TextureView& source, C_TextureView& result);
	static void BilinearDownsampling(const C_TextureView& source, C_TextureView& result);
};
} // namespace GLEngine::Renderer