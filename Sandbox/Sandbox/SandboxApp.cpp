#include <GLRenderer/GLFW/GLFWWindowManager.h>
#include <GLRenderer/GLFW/GLFWWindowFactory.h>
#include <GLRenderer/GLFW/OpenGLWindowInfo.h>

#include <DX12Renderer/D3D12WindowFactory.h>
#include <DX12Renderer/D3D12WindowManager.h>
#include <DX12Renderer/D3D12WindowInfo.h>


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
		using namespace GLEngine::GLRenderer::GLFW;
		auto& logging = Utils::Logging::C_LoggingSystem::Instance();
		logging.AddLogger(new Utils::Logging::C_CoutLogger());
		//logging.AddLogger(new Core::Logging::C_FileLogger("log.txt"));
		S_OpenGLWindowInfo info(640, 480);
		info.m_name = "My title";
		info.m_WindowClass = "ExperimentWindow";
		info.m_MinorVersion = 4;

		m_WndMgr = ConstructGLFWManager(std::bind(&C_Application::OnEvent, this, std::placeholders::_1));
		m_WndMgr->AddWindowFactory(ConstructGLFWWindowFactory());
		m_MainWindow = m_WndMgr->OpenNewWindow(info);
	}

	//=================================================================================
	virtual void Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) override
	{
		using namespace GLEngine::DX12Renderer;
		auto& logging = Utils::Logging::C_LoggingSystem::Instance();
		logging.AddLogger(new Utils::Logging::C_CoutLogger());

		S_D3D12WindowInfo info(640, 480);
		info.m_name = "My title";
		info.m_WindowClass = "D3D12";

		m_WndMgr = ConstructD3D12Manager(std::bind(&C_Application::OnEvent, this, std::placeholders::_1), hInstance);
		m_WndMgr->AddWindowFactory(ConstructD3D12WindowFactory());
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