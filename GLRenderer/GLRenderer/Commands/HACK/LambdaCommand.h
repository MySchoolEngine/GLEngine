#pragma once


#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands::HACK {

class C_LambdaCommand : public Renderer::I_RenderCommand {
public:
	explicit C_LambdaCommand(std::function<void(void)> fnc, const std::string& name = "");

	//=================================================================================
	virtual void								  Commit() override;
	virtual E_Type								  GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;


	//=================================================================================
	[[nodiscard]] virtual std::string GetDescriptor() const override;

private:
	std::string				  m_Name;
	std::function<void(void)> m_fnc;
};

} // namespace GLEngine::GLRenderer::Commands::HACK
