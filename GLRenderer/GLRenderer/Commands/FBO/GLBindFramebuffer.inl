#pragma once

namespace GLEngine::GLRenderer::Commands {

//=================================================================================
template <E_FramebufferTarget framebuffer>
C_GLBindFramebuffer<framebuffer>::C_GLBindFramebuffer(GLuint FBO)
	: m_FBO(FBO)
{
}

//=================================================================================
template <E_FramebufferTarget framebuffer> void C_GLBindFramebuffer<framebuffer>::Commit()
{
	glBindFramebuffer(T_FramebufferTarget<framebuffer>::value, m_FBO);
}

//=================================================================================
template <E_FramebufferTarget framebuffer> Renderer::I_RenderCommand::E_Type C_GLBindFramebuffer<framebuffer>::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::StateChange;
}

//=================================================================================
template <E_FramebufferTarget framebuffer> std::shared_ptr<GLEngine::Renderer::I_Resource> C_GLBindFramebuffer<framebuffer>::GetResource() const
{
	return nullptr;
}

} // namespace GLEngine::GLRenderer::Commands