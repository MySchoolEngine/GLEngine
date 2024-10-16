#pragma once

#include <Renderer/IRenderCommand.h>

#include <Core/CoreMacros.h>

namespace GLEngine::GLRenderer::Commands {

// @todo: Not a final state
class C_GLUseProgram : public Renderer::I_RenderCommand {
public:
	explicit C_GLUseProgram(GLuint program);

	//===========================================
	// Renderer::I_RenderCommand
	//===========================================
	virtual void								  Commit() override;
	virtual E_Type								  GetType() const override;
	[[nodiscard]] virtual std::string			  GetDescriptor() const override;

private:
	GLuint m_Proram;
};

} // namespace GLEngine::GLRenderer::Commands