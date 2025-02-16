#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer::Commands::HACK {
//=================================================================================
C_LambdaCommand::C_LambdaCommand(const std::function<void(void)>& fnc, const std::string& name)
	: m_Name(name)
	, m_fnc(fnc)
{
}

//=================================================================================
void C_LambdaCommand::Commit()
{
	// even though this is hack, I would like to stay sane
	Core::C_Application::Get().GetActiveRenderer().Lock(true);
	m_fnc();
	Core::C_Application::Get().GetActiveRenderer().Lock(false);
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_LambdaCommand::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::DrawCall;
}

//=================================================================================
std::string C_LambdaCommand::GetDescriptor() const
{
	return m_Name;
}

} // namespace GLEngine::GLRenderer::Commands::HACK
