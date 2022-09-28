#pragma once

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands {

template <E_FramebufferTarget framebuffer> class C_GLBindFramebuffer : public Renderer::I_RenderCommand {
public:
	explicit C_GLBindFramebuffer(GLuint FBO);


	//=================================================================================
	// Renderer::I_RenderCommand
	//=================================================================================
	virtual void								  Commit() override;
	virtual E_Type								  GetType() const override;


	//=================================================================================
	[[nodiscard]] virtual std::string GetDescriptor() const override { return std::string("BindFramebuffer") + std::to_string(m_FBO); }

private:
	GLuint m_FBO;
};

} // namespace GLEngine::GLRenderer::Commands

#include <GLRenderer/Commands/FBO/GLBindFramebuffer.inl>