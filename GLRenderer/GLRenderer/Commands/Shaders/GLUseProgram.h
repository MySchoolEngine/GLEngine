#pragma once

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands {

// @todo: Not a final state
class C_GLUseProgram : public Renderer::I_RenderCommand {
public:
	explicit C_GLUseProgram(GLuint program);

	//===========================================
	// Renderer::I_RenderCommand
	//===========================================
	void					  Commit() override;
	E_Type					  GetType() const override;
	[[nodiscard]] std::string GetDescriptor() const override;

private:
	GLuint m_Program;
};

} // namespace GLEngine::GLRenderer::Commands