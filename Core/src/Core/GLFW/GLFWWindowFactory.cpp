#include <CoreStdafx.h>

#include <Core/GLFW/GLFWWindowFactory.h>

#include <Core/GLFW/GLFWoGLWindow.h>

namespace Core {
namespace GLFW {

//=================================================================================
std::shared_ptr<I_Window> C_GLFWWindowFactory::GetWindow(const S_WindowInfo& wndInfo) const
{
	switch (wndInfo.GetDriver())
	{
	case S_WindowInfo::E_Driver::OpenGL:
		return std::make_shared<C_GLFWoGLWindow>(wndInfo);
		break;
	case S_WindowInfo::E_Driver::Vulkan:
	case S_WindowInfo::E_Driver::DirectX:
	default:
		return nullptr;
		break;
	}
}

}
}