#pragma once

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands {
class C_GLBindTexture final : public Renderer::I_RenderCommand {
public:
	explicit C_GLBindTexture(GLenum target, GLuint texture);
	static C_GLBindTexture UnbindTarget(GLenum target);

	//=================================================================================
	void   Commit() override;
	E_Type GetType() const override;

	//=================================================================================
	[[nodiscard]] std::string GetDescriptor() const override;

private:
	GLenum m_Target;
	GLuint m_Texture;
};

} // namespace GLEngine::GLRenderer::Commands