#pragma once

#include <GLRenderer/Textures/Texture.h>

namespace GLEngine::GLRenderer::Commands{

//=================================================================================
template<E_FramebufferTarget framebuffer>
C_glFramebufferTexture<framebuffer>::C_glFramebufferTexture(GLenum attachment, std::shared_ptr<Textures::C_Texture> texture)
	: m_Attachement(attachment)
	, m_Texture(texture)
{

}

//=================================================================================
template<E_FramebufferTarget framebuffer>
void C_glFramebufferTexture<framebuffer>::Commit()
{
	glFramebufferTexture(T_FramebufferTarget<framebuffer>::value, m_Attachement, m_Texture->GetTexture(), 0);
}

//=================================================================================
template<E_FramebufferTarget framebuffer>
Renderer::I_RenderCommand::E_Type C_glFramebufferTexture<framebuffer>::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::StateChange;
}

//=================================================================================
template<E_FramebufferTarget framebuffer>
std::shared_ptr<Renderer::I_Resource> C_glFramebufferTexture<framebuffer>::GetResource() const
{
	return nullptr;
}

}