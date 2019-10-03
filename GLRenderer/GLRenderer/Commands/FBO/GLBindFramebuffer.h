#pragma once

#include <Renderer/IRenderCommand.h>

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

namespace GLEngine::GLRenderer::Commands {

template<E_FramebufferTarget framebuffer>
class C_GLBindFramebuffer : public Renderer::I_RenderCommand {
public:
	C_GLBindFramebuffer(GLuint FBO);


	//=================================================================================
	// Renderer::I_RenderCommand
	//=================================================================================
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;

private:
	E_FramebufferTarget m_Target;
	GLuint				m_FBO;
};

}

#include <GLRenderer/Commands/FBO/GLBindFramebuffer.inl>