#pragma once

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands {

template <E_FramebufferTarget Framebuffer> class C_GLBindFramebuffer final : public Renderer::I_RenderCommand {
public:
	explicit C_GLBindFramebuffer(GLuint FBO);


	//=================================================================================
	// Renderer::I_RenderCommand
	//=================================================================================
	void   Commit() override;
	E_Type GetType() const override;


	//=================================================================================
	[[nodiscard]] std::string GetDescriptor() const override { return std::string("BindFramebuffer") + std::to_string(m_FBO); }

private:
	GLuint m_FBO;
};

} // namespace GLEngine::GLRenderer::Commands

#include <GLRenderer/Commands/FBO/GLBindFramebuffer.inl>