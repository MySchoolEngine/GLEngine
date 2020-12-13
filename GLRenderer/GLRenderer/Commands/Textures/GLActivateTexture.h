#pragma once

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands {
class C_GLActivateTexture final : public Renderer::I_RenderCommand {
public:
	explicit C_GLActivateTexture(unsigned int unit);

	//=================================================================================
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override
	{
		return nullptr;
	}

	//=================================================================================
	[[nodiscard]] virtual std::string GetDescriptor() const override;

private:
	unsigned int m_Unit;
};

}