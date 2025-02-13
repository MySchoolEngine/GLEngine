#pragma once

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands {

class C_GLMakeTextureHandleResident final : public Renderer::I_RenderCommand {
public:
	C_GLMakeTextureHandleResident(std::uint64_t handle, bool makeResident);

	//=================================================================================
	void					  Commit() override;
	E_Type					  GetType() const override { return E_Type::StateChange; }
	[[nodiscard]] std::string GetDescriptor() const override;

private:
	std::uint64_t m_Handle;
	bool		  m_MakeResident;
};

} // namespace GLEngine::GLRenderer::Commands