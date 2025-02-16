#pragma once

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/OGLRenderer.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer::Commands {

//=================================================================================
template <E_FramebufferTarget framebuffer>
C_glFramebufferTexture<framebuffer>::C_glFramebufferTexture(const GLenum attachment, const Renderer::Handle<Renderer::Texture> texture)
	: m_Attachment(attachment)
	, m_Texture(texture)
{
}

//=================================================================================
template <E_FramebufferTarget framebuffer> void C_glFramebufferTexture<framebuffer>::Commit()
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	auto& glRM = dynamic_cast<C_OGLRenderer&>(renderer).GetRMGR();
	auto* texture = glRM.GetTexture(m_Texture);

	glFramebufferTexture(T_FramebufferTarget<framebuffer>::value, m_Attachment, texture->GetTexture(), 0);
}

//=================================================================================
template <E_FramebufferTarget framebuffer> Renderer::I_RenderCommand::E_Type C_glFramebufferTexture<framebuffer>::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::StateChange;
}

//=================================================================================
template <E_FramebufferTarget framebuffer> std::string GLEngine::GLRenderer::Commands::C_glFramebufferTexture<framebuffer>::GetDescriptor() const
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	auto& glRM = dynamic_cast<C_OGLRenderer&>(renderer).GetRMGR();
	auto* texture = glRM.GetTexture(m_Texture);
	return fmt::format("glFramebufferTexture {} {}", m_Attachment, texture->GetTexture());
}

} // namespace GLEngine::GLRenderer::Commands