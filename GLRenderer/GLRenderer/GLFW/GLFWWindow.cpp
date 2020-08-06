#include <GLRendererStdafx.h>

#include <GLRenderer/GLFW/GLFWWindow.h>

#include <Core/EventSystem/Event/AppEvent.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>
#include <Core/Input.h>

namespace GLEngine {
namespace GLRenderer {
namespace GLFW {

//=================================================================================
C_GLFWWindow::C_GLFWWindow()
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
	auto height	= static_cast<int>(wndInfo.m_height);
	auto width	= static_cast<int>(wndInfo.m_width);
	GLFWmonitor* primary = glfwGetPrimaryMonitor();

	if (wndInfo.m_Maximalize)
	{
		int xpos, ypos;
		glfwGetMonitorWorkarea(primary, &xpos, &ypos, &width, &height);
	}

	if (!wndInfo.m_Fullscreen)
	{
		primary = nullptr;
	}

	m_Window = glfwCreateWindow(width, height, wndInfo.m_name.c_str(), primary, nullptr);

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
		if (action == GLFW_PRESS) {
			Core::C_KeyPressedEvent event(key, data.m_GUID);
			data.m_EventCallback(event);
		}
		if (action == GLFW_REPEAT) {
			Core::C_KeyRepeatedEvent event(key, data.m_GUID);
			data.m_EventCallback(event);
		}
		if (action == GLFW_RELEASE) {
			Core::C_KeyReleasedEvent event(key, data.m_GUID);
			data.m_EventCallback(event);
		}
	};

	const auto text_callback = [](GLFWwindow* window, unsigned int codepoint) {
		S_Data& data = *static_cast<S_Data*>(glfwGetWindowUserPointer(window));
		Core::C_TextInputEvent event(codepoint, data.m_GUID);
		data.m_EventCallback(event);
	};

	const auto scroll_callback = [](GLFWwindow* window, double xoffset, double yoffset) {
		S_Data& data = *static_cast<S_Data*>(glfwGetWindowUserPointer(window));
		Core::C_MouseScrollEvent event(xoffset, yoffset, data.m_GUID);
		data.m_EventCallback(event);
	};

	const auto mouse_callback = [](GLFWwindow* window, int button, int action, int mods) {
		S_Data& data = *static_cast<S_Data*>(glfwGetWindowUserPointer(window));
		if (action == GLFW_PRESS) {
			Core::C_MouseButtonPressed event(button, data.m_GUID);
			data.m_EventCallback(event);
		}
		else {
			Core::C_MouseButtonReleased event(button, data.m_GUID);
			data.m_EventCallback(event);
		}
	};

	const auto resize_window_callback = [](GLFWwindow* window, int width, int height) {
		S_Data& data = *static_cast<S_Data*>(glfwGetWindowUserPointer(window));

		Core::C_WindowResizedEvent event(width, height);
		data.m_EventCallback(event);
	};



	const auto mouse_moved_callback = [](GLFWwindow* window, double xPos, double yPos){
		S_Data& data = *static_cast<S_Data*>(glfwGetWindowUserPointer(window));

		Core::C_MouseMoved event((float)xPos, (float)yPos, data.m_GUID);
		data.m_EventCallback(event);
	};

	glfwSetKeyCallback(m_Window, key_callback);
	glfwSetScrollCallback(m_Window, scroll_callback);
	glfwSetMouseButtonCallback(m_Window, mouse_callback);
	glfwSetCursorPosCallback(m_Window, mouse_moved_callback);
	glfwSetCharCallback(m_Window, text_callback);
	glfwSetWindowSizeCallback(m_Window, resize_window_callback);

	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(0);

	m_Input.SetWindow(m_Window);
}

//=================================================================================
const Core::I_Input& C_GLFWWindow::GetInput() const
{
	return m_Input;
}

//=================================================================================
glm::vec2 C_GLFWWindow::ToClipSpace(const glm::vec2& screenCoord) const
{
	const float x = (2.0f * screenCoord.x) / GetWidth() - 1.0f;
	const float y = 1.0f - (2.0f * screenCoord.y) / GetHeight();
	return { x, y };
}

//=================================================================================
void C_GLFWWindow::Destroy()
{
	glfwDestroyWindow(m_Window);
}

}
}
}

