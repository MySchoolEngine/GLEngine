#include <CoreStdafx.h>

#include <Core/GLFW/GLFWWindowManager.h>

#include <Core/GLFW/GLFWWindow.h>

#include <GLFW/glfw3.h>

namespace Core {

//=================================================================================
C_GLFWWindowManager::C_GLFWWindowManager()
{
	Init();
}

//=================================================================================
std::shared_ptr<I_Window> C_GLFWWindowManager::OpenNewWindow(const S_WindowInfo& info)
{
	auto window = std::make_shared<C_GLFWWindow>();
	window->SetTitle(info.m_name);
	m_Windows.push_back(window);
	return window;
}

//=================================================================================
void C_GLFWWindowManager::Update()
{
	std::for_each(m_Windows.begin(), m_Windows.end(), [](const std::shared_ptr<I_Window> window) {window->Update(); });

	glfwPollEvents();
}

//=================================================================================
void C_GLFWWindowManager::Init()
{
	const auto error_callback = [](int error, const char* description) {
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: Error(%d)", error);
	};
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: Unable to init glfw. Terminating engine");
		exit(EXIT_FAILURE);
	}
}

}