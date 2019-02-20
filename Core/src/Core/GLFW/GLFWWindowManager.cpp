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
	//glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
}

}