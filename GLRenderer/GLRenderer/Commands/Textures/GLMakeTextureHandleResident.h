#pragma once

#include <Renderer/IRenderCommand.h>

#include <Core/CoreMacros.h>

namespace GLEngine::GLRenderer::Commands {

class C_GLMakeTextureHandleResident final : public Renderer::I_RenderCommand {
public:
	C_GLMakeTextureHandleResident(std::uint64_t handle, bool makeResident);

	//=================================================================================
	virtual void								  Commit() override;
	virtual E_Type								  GetType() const override { return E_Type::StateChange; }
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override { return nullptr; }
	[[nodiscard]] virtual std::string			  GetDescriptor() const override;

private:
	std::uint64_t m_Handle;
	bool		  m_MakeResident;
};

} // namespace GLEngine::GLRenderer::Commands