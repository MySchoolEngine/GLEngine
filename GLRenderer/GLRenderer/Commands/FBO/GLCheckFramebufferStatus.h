#pragma once

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands {

template <E_FramebufferTarget framebuffer> class C_GLCheckFramebufferStatus : public Renderer::I_RenderCommand {
public:
	explicit C_GLCheckFramebufferStatus(std::promise<bool>&& prom);

	//=================================================================================
	// Renderer::I_RenderCommand
	//=================================================================================
	virtual void								  Commit() override;
	virtual E_Type								  GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;


	//=================================================================================
	[[nodiscard]] virtual std::string GetDescriptor() const override { return std::string("CheckFramebufferStatus"); }

private:
	std::promise<bool> m_Promise;
};

} // namespace GLEngine::GLRenderer::Commands

#include <GLRenderer/Commands/FBO/GLCheckFramebufferStatus.inl>