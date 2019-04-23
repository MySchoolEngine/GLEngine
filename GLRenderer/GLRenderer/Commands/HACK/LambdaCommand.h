#pragma once


#include <Renderer/IRenderCommand.h>

#include <memory>

namespace GLEngine {
namespace GLRenderer {
namespace Commands {
namespace HACK {

class C_LambdaCommand : public Renderer::I_RenderCommand {
public:
	C_LambdaCommand(std::function<void(void)> fnc);

	//=================================================================================
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;

private:
	std::function<void(void)> m_fnc;
};

}}}}

