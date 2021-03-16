#pragma once

namespace GLEngine::GLRenderer::Commands {

//=================================================================================
template <E_FramebufferTarget framebuffer>
C_GLCheckFramebufferStatus<framebuffer>::C_GLCheckFramebufferStatus(std::promise<bool>&& prom)
	: m_Promise(std::move(prom))
{
}

//=================================================================================
template <E_FramebufferTarget framebuffer> std::shared_ptr<GLEngine::Renderer::I_Resource> C_GLCheckFramebufferStatus<framebuffer>::GetResource() const
{
	return nullptr;
}

//=================================================================================
template <E_FramebufferTarget framebuffer> Renderer::I_RenderCommand::E_Type C_GLCheckFramebufferStatus<framebuffer>::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::StateChange;
}

//=================================================================================
template <E_FramebufferTarget framebuffer> void C_GLCheckFramebufferStatus<framebuffer>::Commit()
{
	auto status = glCheckFramebufferStatus(T_FramebufferTarget<framebuffer>::value);
	m_Promise.set_value(status == GL_FRAMEBUFFER_COMPLETE);
}
} // namespace GLEngine::GLRenderer::Commands
