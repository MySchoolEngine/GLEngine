#include <CoreStdafx.h>

#include <Core/GLFW/GLFWoGLWindow.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Core {
namespace GLFW {

//=================================================================================
C_GLFWoGLWindow::C_GLFWoGLWindow()
{
	Init();
}

//=================================================================================
void C_GLFWoGLWindow::Update()
{
	glfwMakeContextCurrent(m_Window);
	glClearColor(m_color, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_color += 0.05f;
	if (m_color > 1.0f) m_color = 0.0f;
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

//=================================================================================
void C_GLFWoGLWindow::Init()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	C_GLFWWindow::Init();
	glfwMakeContextCurrent(m_Window);
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (status==0) {
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: Glad wasn't loaded properlly. Status {}", status);
	}

	CORE_LOG(E_Level::Info, E_Context::Render, "GLFW: OpenGL window initialized");
}

}
}


