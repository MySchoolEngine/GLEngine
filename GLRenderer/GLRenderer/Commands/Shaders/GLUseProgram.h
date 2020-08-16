#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderCommand.h>

namespace GLEngine {
namespace GLRenderer {
namespace Commands {

// @todo: Not a final state
class C_GLUseProgram : public Renderer::I_RenderCommand {
public:
	explicit C_GLUseProgram(GLuint program);

	//===========================================
	// Renderer::I_RenderCommand
	//===========================================
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;
	[[nodiscard]] virtual std::string GetDescriptor() const override;

private:
	GLuint m_Proram;
};

}}}