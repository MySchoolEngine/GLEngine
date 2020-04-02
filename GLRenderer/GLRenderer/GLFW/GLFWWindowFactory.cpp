#include <GLRendererStdafx.h>

#include <GLRenderer/GLFW/GLFWWindowFactory.h>

#include <GLRenderer/GLFW/GLFWoGLWindow.h>
#include <GLRenderer/Windows/ExperimentWindow.h>
#include <GLRenderer/Windows/SpectralData.h>

namespace GLEngine {
namespace GLRenderer {
namespace GLFW {

//=================================================================================
std::shared_ptr<Core::I_Window> C_GLFWWindowFactory::GetWindow(const Core::S_WindowInfo& wndInfo) const
{
	if (wndInfo.m_WindowClass == "GLFW_OGL") {
		return std::make_shared<C_GLFWoGLWindow>(wndInfo);
	}
	if (wndInfo.m_WindowClass == "ExperimentWindow") {
		return std::make_shared<Windows::C_ExplerimentWindow>(wndInfo);
	}
	if (wndInfo.m_WindowClass == "SpectralData") {
		return std::make_shared<Windows::C_SpectralData>(wndInfo);
	}
	return nullptr;
}

//=================================================================================
Core::I_WindowFactory* ConstructGLFWWindowFactory()
{
	return new C_GLFWWindowFactory();
}

}
}
}