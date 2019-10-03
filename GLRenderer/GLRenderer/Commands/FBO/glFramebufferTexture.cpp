#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/FBO/glFramebufferTexture.h>

#include <GLRenderer/Textures/Texture.h>

namespace GLEngine::GLRenderer::Commands{

//=================================================================================
C_glFramebufferTexture::C_glFramebufferTexture(GLenum attachment, std::shared_ptr<Textures::C_Texture> texture)
	: m_Attachement(attachment)
	, m_Texture(texture)
{

}

//=================================================================================
void C_glFramebufferTexture::Commit()
{
	glFramebufferTexture(GL_FRAMEBUFFER, m_Attachement, m_Texture->GetTexture(), 0);
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_glFramebufferTexture::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::StateChange;
}

//=================================================================================
std::shared_ptr<Renderer::I_Resource> C_glFramebufferTexture::GetResource() const
{
	return nullptr;
}

}