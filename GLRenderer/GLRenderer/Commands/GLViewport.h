#pragma once

#include <Renderer/IRenderCommand.h>
#include <Renderer/Viewport.h>

namespace GLEngine::GLRenderer::Commands {
class C_GLViewport final : public Renderer::I_RenderCommand {
public:
	C_GLViewport(const Renderer::C_Viewport& viewport);

	//======================================
	// Renderer::I_RenderCommand
	//======================================
	void								  Commit() override;
	E_Type								  GetType() const override;
	[[nodiscard]] std::string			  GetDescriptor() const override;

private:
	Renderer::C_Viewport m_Viewport;
};
} // namespace GLEngine::GLRenderer::Commands