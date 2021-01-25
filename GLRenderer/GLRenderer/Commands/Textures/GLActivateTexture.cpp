#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/Textures/GLActivateTexture.h>

namespace GLEngine::GLRenderer::Commands {

//=================================================================================
C_GLActivateTexture::C_GLActivateTexture(unsigned int unit)
	: m_Unit(unit)
{
}

//=================================================================================
void C_GLActivateTexture::Commit()
{
	glActiveTexture(GL_TEXTURE0 + m_Unit);
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_GLActivateTexture::GetType() const
{
	return E_Type::StateChange;
}

//=================================================================================
std::string C_GLActivateTexture::GetDescriptor() const
{
	return fmt::format("ActivateTexture unit: {}", m_Unit);
}

} // namespace GLEngine::GLRenderer::Commands