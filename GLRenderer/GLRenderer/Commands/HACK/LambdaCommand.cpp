#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine {
namespace GLRenderer {
namespace Commands {
namespace HACK {


//=================================================================================
C_LambdaCommand::C_LambdaCommand(std::function<void(void)> fnc, const std::string& name)
	: m_fnc(fnc)
	, m_Name(name)
{

}

//=================================================================================
void C_LambdaCommand::Commit()
{
	// even though this is hack, I would like to stay sane
	Core::C_Application::Get().GetActiveRenderer()->Lock(true);
	m_fnc();
	Core::C_Application::Get().GetActiveRenderer()->Lock(false);
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_LambdaCommand::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::DrawCall;
}

//=================================================================================
std::shared_ptr<Renderer::I_Resource> C_LambdaCommand::GetResource() const
{
	return nullptr;
}

//=================================================================================
std::string C_LambdaCommand::GetDescriptor() const
{
	return m_Name;
}

}}}}
