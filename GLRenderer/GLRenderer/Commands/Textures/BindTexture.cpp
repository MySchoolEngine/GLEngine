#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/Textures/BindTexture.h>

namespace GLEngine::GLRenderer::Commands {

//=================================================================================
C_GLBindTexture::C_GLBindTexture(GLenum target, GLuint texture)
	: m_Target(target)
	, m_Texture(texture)
{}

//=================================================================================
void C_GLBindTexture::Commit()
{
	glBindTexture(m_Target, m_Texture);
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_GLBindTexture::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::StateChange;
}

//=================================================================================
std::string C_GLBindTexture::GetDescriptor() const
{
	return fmt::format("Bind texture '{}' with target '{}'", m_Texture, m_Target);
}

//=================================================================================
C_GLBindTexture C_GLBindTexture::UnbindTarget(GLenum target)
{
	return C_GLBindTexture(target, 0);
}

}
