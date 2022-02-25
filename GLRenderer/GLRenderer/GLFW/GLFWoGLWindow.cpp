#include <GLRendererStdafx.h>

#include <GLRenderer/GLFW/GLFWoGLWindow.h>
#include <GLRenderer/GLFW/OpenGLWindowInfo.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/OGLDevice.h>

#include <Renderer/IRenderer.h>

namespace GLEngine::GLRenderer::GLFW {

//=================================================================================
C_GLFWoGLWindow::C_GLFWoGLWindow(const Core::S_WindowInfo& wndInfo)
	: m_renderer(nullptr)
{
	Init(wndInfo);
}

//=================================================================================
C_GLFWoGLWindow::~C_GLFWoGLWindow() = default;

//=================================================================================
void C_GLFWoGLWindow::Update()
{
	glfwMakeContextCurrent(m_Window);
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

//=================================================================================
void C_GLFWoGLWindow::Init(const Core::S_WindowInfo& wndInfo)
{
	GLE_ASSERT(wndInfo.GetDriver() == Core::E_Driver::OpenGL, "This class supports only OpenGL");

	const auto wndInfoOGL = dynamic_cast<const S_OpenGLWindowInfo*>(&wndInfo);
	WindowHint(GLFW_CONTEXT_VERSION_MAJOR, wndInfoOGL->m_MajorVersion);
	WindowHint(GLFW_CONTEXT_VERSION_MINOR, wndInfoOGL->m_MinorVersion);
	WindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#ifdef GL_ENGINE_DEBUG
	WindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	SetLayerDebugName(wndInfo.m_name);

	C_GLFWWindow::Init(wndInfo);
	MakeCurrent();

	m_Device				= std::make_unique<GLRenderer::C_GLDevice>();
	m_renderer				= std::make_unique<GLEngine::GLRenderer::C_OGLRenderer>(*(m_Device.get()));
	const char* description = nullptr;
	const auto	error		= glfwGetError(&description);
	if (error != GLFW_NO_ERROR)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "GLFW: OpenGL window init failed: {}", description);
	}

	CORE_LOG(E_Level::Info, E_Context::Render, "GLFW: OpenGL window initialized");
}

//=================================================================================
Renderer::I_Renderer& C_GLFWoGLWindow::GetRenderer()
{
	return *(m_renderer.get());
}

//=================================================================================
void C_GLFWoGLWindow::OnEvent(Core::I_Event& event)
{
}

} // namespace GLEngine::GLRenderer::GLFW