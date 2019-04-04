

#include <GLRenderer/GLFW/GLFWWindow.h>
#include <GLRenderer/GLFW/GLFWWindowManager.h>
#include <GLRenderer/GLFW/GLFWWindowFactory.h>
#include <GLRenderer/GLFW/OpenGLWindowInfo.h>


#include <Utils/Logging/Logging.h>
#include <Utils/Logging/ILogger.h>
#include <Utils/Logging/LoggingMacros.h>

#include <Core/Application.h>


#include <Core/EntryPoint.h>



class SandboxApp : public GLEngine::Core::C_Application {
public:
	//=================================================================================
	virtual void Init() override
	{
		auto& logging = Utils::Logging::C_LoggingSystem::Instance();
		logging.AddLogger(new Utils::Logging::C_CoutLogger());
		//logging.AddLogger(new Core::Logging::C_FileLogger("log.txt"));
		GLEngine::GLRenderer::GLFW::S_OpenGLWindowInfo info(640, 480);
		info.m_name = "My title";
		info.m_WindowClass = "ExperimentWindow";
		info.m_MinorVersion = 0;

		m_WndMgr = GLEngine::GLRenderer::GLFW::ConstructGLFWManager(std::bind(&C_Application::OnEvent, this, std::placeholders::_1));
		m_WndMgr->AddWindowFactory(GLEngine::GLRenderer::GLFW::ConstructGLFWWindowFactory());
		m_MainWindow = m_WndMgr->OpenNewWindow(info);
	}



	//=================================================================================
	virtual const std::unique_ptr<GLEngine::Renderer::I_Renderer>& GetActiveRenderer() const override
	{
		return m_MainWindow->GetRenderer();
	}

protected:

	//=================================================================================
	virtual GLEngine::Core::I_WindowManager& GetWndMgr() const override
	{
		return *m_WndMgr;
	}
private:
	GLEngine::Core::I_WindowManager* m_WndMgr;
	std::shared_ptr<GLEngine::Core::I_Window> m_MainWindow;
};

GLEngine::Core::C_Application* GLEngine::Core::CreateApplication() {
	return new SandboxApp();
}