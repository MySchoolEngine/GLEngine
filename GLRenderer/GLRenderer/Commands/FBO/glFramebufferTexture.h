#pragma once

#include <Renderer/IRenderCommand.h>
#include <Renderer/Resources/RenderResourceHandle.h>

namespace GLEngine::GLRenderer::Commands {
template <E_FramebufferTarget framebuffer> class C_glFramebufferTexture final : public Renderer::I_RenderCommand {
public:
	C_glFramebufferTexture(const GLenum attachment, const Renderer::Handle<Renderer::Texture> texture);


	//=================================================================================
	// Renderer::I_RenderCommand
	//=================================================================================
	void					  Commit() override;
	E_Type					  GetType() const override;
	[[nodiscard]] std::string GetDescriptor() const override;

private:
	GLenum								m_Attachment;
	Renderer::Handle<Renderer::Texture> m_Texture;
};

} // namespace GLEngine::GLRenderer::Commands

#include <GLRenderer/Commands/FBO/glFramebufferTexture.inl>