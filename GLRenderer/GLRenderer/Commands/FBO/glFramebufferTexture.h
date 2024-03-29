#pragma once

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Textures {
class C_Texture;
}

namespace GLEngine::GLRenderer::Commands {
template <E_FramebufferTarget framebuffer> class C_glFramebufferTexture : public Renderer::I_RenderCommand {
public:
	C_glFramebufferTexture(GLenum attachment, std::shared_ptr<Textures::C_Texture> texture);


	//=================================================================================
	// Renderer::I_RenderCommand
	//=================================================================================
	virtual void								  Commit() override;
	virtual E_Type								  GetType() const override;
	[[nodiscard]] virtual std::string			  GetDescriptor() const override;

private:
	GLenum								 m_Attachement;
	std::shared_ptr<Textures::C_Texture> m_Texture;
};

} // namespace GLEngine::GLRenderer::Commands

#include <GLRenderer/Commands/FBO/glFramebufferTexture.inl>