#pragma once

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands {
class C_GLActivateTexture final : public Renderer::I_RenderCommand {
public:
	explicit C_GLActivateTexture(unsigned int unit);

	//=================================================================================
	virtual void								  Commit() override;
	virtual E_Type								  GetType() const override;

	//=================================================================================
	[[nodiscard]] virtual std::string GetDescriptor() const override;

private:
	unsigned int m_Unit;
};

} // namespace GLEngine::GLRenderer::Commands