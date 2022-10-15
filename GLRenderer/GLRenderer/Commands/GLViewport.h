#pragma once

#include <Renderer/IRenderCommand.h>
#include <Renderer/Viewport.h>

namespace GLEngine::GLRenderer::Commands {
class C_GLViewport : public Renderer::I_RenderCommand {
public:
	C_GLViewport(const Renderer::C_Viewport& viewport);

	//======================================
	// Renderer::I_RenderCommand
	//======================================
	virtual void								  Commit() override;
	virtual E_Type								  GetType() const override;
	[[nodiscard]] virtual std::string			  GetDescriptor() const override;

private:
	Renderer::C_Viewport m_viewport;
};
} // namespace GLEngine::GLRenderer::Commands