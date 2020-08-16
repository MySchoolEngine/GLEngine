#pragma once

#include <Renderer/IRenderCommand.h>

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

namespace GLEngine::GLRenderer::Commands {

template<E_FramebufferTarget framebuffer>
class C_GLBindFramebuffer : public Renderer::I_RenderCommand {
public:
	explicit C_GLBindFramebuffer(GLuint FBO);


	//=================================================================================
	// Renderer::I_RenderCommand
	//=================================================================================
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;


	//=================================================================================
	[[nodiscard]] virtual std::string GetDescriptor() const override
	{
		return std::string("BindFramebuffer") + std::to_string(m_FBO);
	}

private:
	GLuint				m_FBO;
};

}

#include <GLRenderer/Commands/FBO/GLBindFramebuffer.inl>