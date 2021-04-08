#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/ShaderResource.h>


namespace GLEngine::GLRenderer::Shaders {

//=================================================================================
std::unique_ptr<Renderer::I_RawGPUData> C_ShaderResource::ExtractData() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_ShaderResource::Invalidate()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
bool C_ShaderResource::IsValid()
{
	throw std::logic_error("The method or operation is not implemented.");
}

} // namespace GLEngine::GLRenderer::Shaders
