#pragma once

#include <Renderer/IRenderer.h>
#include <Renderer/RendererApi.h>

#include <memory>

namespace GLEngine { namespace Renderer {

class RENDERER_API_EXPORT F_RendererFacotry {
public:
	static std::unique_ptr<Renderer::I_Renderer> GetRenderer();
};
}} // namespace GLEngine::Renderer
