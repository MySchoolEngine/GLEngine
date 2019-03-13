#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderer.h>

#include <memory>

namespace GLEngine {
namespace Renderer {

class API_EXPORT F_RendererFacotry {
public:
	static std::unique_ptr<Renderer::I_Renderer> GetRenderer();
};
}
}

