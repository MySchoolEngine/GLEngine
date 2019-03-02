#include <CoreStdafx.h>

#include <Core/GLFW/GLFWWindow.h>


#include <GLFW/glfw3.h>

namespace Core {
namespace GLFW {

//=================================================================================
C_GLFWWindow::C_GLFWWindow()
	: m_Window(nullptr)
{
	Init();
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
void C_GLFWWindow::Init()
{

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	m_Window = glfwCreateWindow(640, 480, "Simple example", nullptr, nullptr);

	if (!m_Window)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: Unable to open a window. Shuting down the engine.");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	const auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		CORE_LOG(E_Level::Info, E_Context::Core, "{} {}", key, scancode);
	};

	glfwSetKeyCallback(m_Window, key_callback);

	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(1);
}

//=================================================================================
void C_GLFWWindow::Destroy()
{
	glfwDestroyWindow(m_Window);
}

}
}

