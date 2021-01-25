#include <GLRendererStdafx.h>

#include <GLRenderer/GLFW/GLFWWindowFactory.h>
#include <GLRenderer/GLFW/GLFWoGLWindow.h>
#include <GLRenderer/Windows/ExperimentWindow.h>

namespace GLEngine::GLRenderer::GLFW {

//=================================================================================
std::shared_ptr<Core::I_Window> C_GLFWWindowFactory::GetWindow(const Core::S_WindowInfo& wndInfo) const
{
	if (wndInfo.m_WindowClass == "GLFW_OGL")
	{
		return std::make_shared<C_GLFWoGLWindow>(wndInfo);
	}
	if (wndInfo.m_WindowClass == "ExperimentWindow")
	{
		return std::make_shared<Windows::C_ExplerimentWindow>(wndInfo);
	}
	return nullptr;
}

//=================================================================================
Core::I_WindowFactory* ConstructGLFWWindowFactory()
{
	return new C_GLFWWindowFactory();
}

} // namespace GLEngine::GLRenderer::GLFW