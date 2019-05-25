#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderCommand.h>

namespace GLEngine {
namespace GLRenderer {
namespace Commands {

class C_GLActivateTexture final : public Renderer::I_RenderCommand {
public:
	C_GLActivateTexture(unsigned int unit);

	//=================================================================================
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override
	{
		return nullptr;
	}

private:
	unsigned int m_Unit;
};

}}}