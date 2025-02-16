#pragma once


#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands::HACK {

class C_LambdaCommand final : public Renderer::I_RenderCommand {
public:
	explicit C_LambdaCommand(const std::function<void(void)>& fnc, const std::string& name = "");

	//=================================================================================
	void								  Commit() override;
	E_Type								  GetType() const override;


	//=================================================================================
	[[nodiscard]] std::string GetDescriptor() const override;

private:
	std::string				  m_Name;
	std::function<void(void)> m_fnc;
};

} // namespace GLEngine::GLRenderer::Commands::HACK
