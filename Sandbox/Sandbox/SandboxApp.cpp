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
#include <Core/WindowManager.h>

#include <Core/EntryPoint.h>

#include <type_traits>


class SandboxApp : public GLEngine::Core::C_Application {
public:

	//=================================================================================
	virtual void Init() override
	{
		InitInt();

		OpenWindwos();
	}

	//=================================================================================
	virtual void Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) override
	{
		InitInt();

		auto* manager = GLEngine::DX12Renderer::ConstructD3D12Manager(std::bind(&C_Application::OnEvent, this, std::placeholders::_1), hInstance);
		manager->AddWindowFactory(GLEngine::DX12Renderer::ConstructD3D12WindowFactory());
		m_WndMgr->AddManager<GLEngine::DX12Renderer::C_D3D12WindowManager>(manager);


		OpenWindwos();
	}

	//=================================================================================
	virtual const std::unique_ptr<GLEngine::Renderer::I_Renderer>& GetActiveRenderer() const override
	{
		return m_WndMgr->GetActiveRenderer();
	}
protected:

	void InitInt()
	{
		auto& logging = Utils::Logging::C_LoggingSystem::Instance();
		logging.AddLogger(new Utils::Logging::C_CoutLogger());
		//logging.AddLogger(new Core::Logging::C_FileLogger("log.txt"));

		m_WndMgr = new std::remove_pointer_t<decltype(m_WndMgr)>(std::bind(&C_Application::OnEvent, this, std::placeholders::_1));

		{
			// we can open glfw window from here
			using namespace GLEngine::GLRenderer::GLFW;
			auto* manager = ConstructGLFWManager(std::bind(&C_Application::OnEvent, this, std::placeholders::_1));
			manager->AddWindowFactory(ConstructGLFWWindowFactory());
			m_WndMgr->AddManager<C_GLFWWindowManager>(manager);
		}
	}

	void OpenWindwos()
	{
		if(false){
			using namespace GLEngine::DX12Renderer;

			S_D3D12WindowInfo info(640, 480);
			info.m_name = "DX Window";
			info.m_WindowClass = "D3D12";

			m_WndMgr->OpenNewWindow(info);
		}


		{
			using namespace GLEngine::GLRenderer::GLFW;

			S_OpenGLWindowInfo info(640, 480);
			info.m_name = "ExperimentWindow";
			info.m_WindowClass = "SpectralData";
			info.m_MinorVersion = 4;

			m_WndMgr->OpenNewWindow(info);
		}
	}

	//=================================================================================
	virtual GLEngine::Core::I_WindowManager& GetWndMgr() const override
	{
		return *m_WndMgr;
	}
private:
	GLEngine::Core::C_WindwoManager<GLEngine::GLRenderer::GLFW::C_GLFWWindowManager, GLEngine::DX12Renderer::C_D3D12WindowManager>* m_WndMgr;
};

GLEngine::Core::C_Application* GLEngine::Core::CreateApplication() {
	return new SandboxApp();
}