#include <CoreStdafx.h>

#include <Core/GLFW/GLFWWindowManager.h>

#include <Core/GLFW/GLFWoGLWindow.h>

#include <GLFW/glfw3.h>

namespace Core {
namespace GLFW {

//=================================================================================
C_GLFWWindowManager::C_GLFWWindowManager()
{
	Init();
}

//=================================================================================
std::shared_ptr<I_Window> C_GLFWWindowManager::OpenNewWindow(const S_WindowInfo& info)
{
	auto window = std::make_shared<C_GLFWoGLWindow>();
	window->SetTitle(info.m_name);
	m_Windows.push_back(window);
	return window;
}

//=================================================================================
void C_GLFWWindowManager::Update()
{
	m_Windows.erase(std::remove_if(m_Windows.begin(), m_Windows.end(), [](const decltype(m_Windows)::value_type window) {
		return window->WantClose();
	}), m_Windows.end());

	std::for_each(m_Windows.begin(), m_Windows.end(), [](const decltype(m_Windows)::value_type window) {window->Update(); });

	glfwPollEvents();
}

//=================================================================================
unsigned int C_GLFWWindowManager::NumWindows() const
{
	return static_cast<unsigned int>(m_Windows.size());
}

//=================================================================================
void C_GLFWWindowManager::Init()
{
	const auto error_callback = [](int error, const char* description) {
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: #{}: {}", error, description);
	};
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: Unable to init glfw. Terminating engine");
		exit(EXIT_FAILURE);
	}
}

}
}