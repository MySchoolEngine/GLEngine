#include <CoreStdafx.h>

#include <Core/GLFW/GLFWWindow.h>


#include <GLFW/glfw3.h>

namespace Core {

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
	glfwPollEvents();
}

//=================================================================================
void C_GLFWWindow::Init()
{

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	m_Window = glfwCreateWindow(640, 480, "Simple example", nullptr, nullptr);

	if (!m_Window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(1);
}

//=================================================================================
void C_GLFWWindow::Destroy()
{
	glfwDestroyWindow(m_Window);
}

}

