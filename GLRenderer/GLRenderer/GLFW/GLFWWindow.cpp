#include <GLRendererStdafx.h>

#include <GLRenderer/GLFW/GLFWWindow.h>

#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>

namespace GLEngine {
namespace GLRenderer {
namespace GLFW {

//=================================================================================
API_EXPORT C_GLFWWindow::C_GLFWWindow()
	: m_Window(nullptr)
{
	m_Data.m_GUID = GetGUID();
}

//=================================================================================
C_GLFWWindow::~C_GLFWWindow()
{
	Destroy();
}

//=================================================================================
unsigned int C_GLFWWindow::GetWidth() const
{
	return GetSize().x;
}

//=================================================================================
unsigned int C_GLFWWindow::GetHeight() const
{
	return GetSize().y;
}

//=================================================================================
glm::uvec2 C_GLFWWindow::GetSize() const
{
	glm::ivec2 size;
	glfwGetWindowSize(m_Window, &(size.x), &(size.y));

	return size;
}

//=================================================================================
void C_GLFWWindow::SetTitle(const std::string& title)
{
	glfwSetWindowTitle(m_Window, title.data());
}

//=================================================================================
void C_GLFWWindow::Update()
{
	glfwMakeContextCurrent(m_Window);
	glfwSwapBuffers(m_Window);
}

//=================================================================================
bool C_GLFWWindow::WantClose() const
{
	return glfwWindowShouldClose(m_Window) != 0;
}

//=================================================================================
void C_GLFWWindow::Init(const Core::S_WindowInfo& wndInfo)
{
	m_Window = glfwCreateWindow(wndInfo.m_width, wndInfo.m_height, wndInfo.m_name.c_str(), nullptr, nullptr);

	if (!m_Window)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: Unable to open a window. Shuting down the engine.");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}


	CORE_LOG(E_Level::Info, E_Context::Core, "GLFW: Window initialized.");
	glfwSetWindowUserPointer(m_Window, &m_Data);

	const auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		S_Data& data = *static_cast<S_Data*>(glfwGetWindowUserPointer(window));
		CORE_LOG(E_Level::Info, E_Context::Core, "Key: {} {}", key, scancode);
		Core::C_KeyPressedEvent event(key, data.m_GUID);
		data.m_EventCallback(event);
	};

	const auto scroll_callback = [](GLFWwindow* window, double xoffset, double yoffset) {
		S_Data& data = *static_cast<S_Data*>(glfwGetWindowUserPointer(window));
		CORE_LOG(E_Level::Info, E_Context::Core, "Scroll: {}x{}", xoffset, yoffset);
		Core::C_MouseScrollEvent event(xoffset, yoffset, data.m_GUID);
		data.m_EventCallback(event);
	};

	glfwSetKeyCallback(m_Window, key_callback);
	glfwSetScrollCallback(m_Window, scroll_callback);

	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(1);
}

//=================================================================================
void C_GLFWWindow::SetEventCallback(Core::C_Application::EventCallbackFn callback)
{
	m_Data.m_EventCallback = callback;
}

//=================================================================================
void C_GLFWWindow::Destroy()
{
	glfwDestroyWindow(m_Window);
}

}
}
}

