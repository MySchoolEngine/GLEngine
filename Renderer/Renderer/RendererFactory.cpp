#include <RendererStdafx.h>

#include <Renderer/RendererFactory.h>

//#include <GLRenderer/OGLRenderer.h>

namespace GLEngine::Renderer {

//=================================================================================
std::unique_ptr<GLEngine::Renderer::I_Renderer> F_RendererFactory::GetRenderer()
{
	return nullptr;
	// return std::make_unique<GLRenderer::C_OGLRenderer>();
}

} // namespace GLEngine::Renderer