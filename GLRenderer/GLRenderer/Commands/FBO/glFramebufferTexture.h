#pragma once

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Textures {
class C_Texture;
}

namespace GLEngine::GLRenderer::Commands {
template<E_FramebufferTarget framebuffer>
class C_glFramebufferTexture : public Renderer::I_RenderCommand {
public:
	C_glFramebufferTexture(GLenum attachment, std::shared_ptr<Textures::C_Texture> texture);


	//=================================================================================
	// Renderer::I_RenderCommand
	//=================================================================================
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;
	[[nodiscard]] virtual std::string GetDescriptor() const override
	{
		return fmt::format("glFramebufferTexture {} {}", m_Attachement, m_Texture->GetTexture());
	}

private:
	GLenum									m_Attachement;
	std::shared_ptr<Textures::C_Texture>	m_Texture;
};

}

#include <GLRenderer/Commands/FBO/glFramebufferTexture.inl>