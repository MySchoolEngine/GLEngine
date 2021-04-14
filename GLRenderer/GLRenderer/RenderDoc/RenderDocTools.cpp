#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/RenderDoc/RenderDocTools.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer::RenderDoc {

//=================================================================================
C_DebugScope::C_DebugScope(const std::string& groupName)
{
	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[groupName]() { glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(groupName.length()), groupName.c_str()); },
		fmt::format("DebugScope begin - {}", groupName)));
}

//=================================================================================
C_DebugScope::~C_DebugScope()
{
	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([]() { glPopDebugGroup(); }, fmt::format("DebugScope end")));
}

} // namespace GLEngine::GLRenderer::RenderDoc