#include <GLRenderer/OGLRenderer.h>

#include <stdexcept>

namespace GLEngine {
namespace GLRenderer {

//=================================================================================
void C_OGLRenderer::AddCommand(std::unique_ptr<Renderer::I_RenderCommand> command)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_OGLRenderer::AddBatch(std::unique_ptr<Renderer::I_RenderBatch> batc)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_OGLRenderer::SortCommands()
{
	// todo: no sorting for now
}

//=================================================================================
void C_OGLRenderer::ExtractData()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_OGLRenderer::TransformData()
{
	// todo: no extraction for now
}

//=================================================================================
void C_OGLRenderer::Commit() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_OGLRenderer::ClearCommandBuffers()
{
	throw std::logic_error("The method or operation is not implemented.");
}

}
}

