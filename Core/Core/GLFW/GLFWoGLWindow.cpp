#include <CoreStdafx.h>

#include <Core/GLFW/GLFWoGLWindow.h>

#include <Renderer/IRenderer.h>

#include <Renderer/RendererFactory.h>

#include <GLFW/glfw3.h>

namespace Core {
namespace GLFW {

//=================================================================================
C_GLFWoGLWindow::C_GLFWoGLWindow(const S_WindowInfo& wndInfo)
	: m_renderer(nullptr)
{
	Init(wndInfo);
}

//=================================================================================
void C_GLFWoGLWindow::Update()
{
	glfwMakeContextCurrent(m_Window);
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

//=================================================================================
void C_GLFWoGLWindow::Init(const S_WindowInfo& wndInfo)
{
	// TODO: add assert here
	const auto wndInfoOGL = dynamic_cast<const S_OpenGLWindowInfo*>(&wndInfo);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, wndInfoOGL->m_MajorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, wndInfoOGL->m_MinorVersion);

	C_GLFWWindow::Init(wndInfo);
	glfwMakeContextCurrent(m_Window);

	m_renderer = GLEngine::Renderer::F_RendererFacotry::GetRenderer();

	CORE_LOG(E_Level::Info, E_Context::Render, "GLFW: OpenGL window initialized");
}

}
}


