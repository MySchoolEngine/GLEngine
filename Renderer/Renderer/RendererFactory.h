#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/IRenderer.h>

#include <memory>

namespace GLEngine {
namespace Renderer {

class RENDERER_API_EXPORT F_RendererFacotry {
public:
  static std::unique_ptr<Renderer::I_Renderer> GetRenderer();
};
} // namespace Renderer
} // namespace GLEngine
