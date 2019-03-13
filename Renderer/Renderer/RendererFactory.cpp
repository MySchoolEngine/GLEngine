#include <Renderer/RendererFactory.h>

#include <GLRenderer/OGLRenderer.h>

namespace GLEngine {
namespace Renderer {

//=================================================================================
std::unique_ptr<GLEngine::Renderer::I_Renderer> F_RendererFacotry::GetRenderer()
{
	return std::make_unique<GLRenderer::C_OGLRenderer>();
}

}
}