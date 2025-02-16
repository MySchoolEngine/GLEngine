#include <SandboxStdafx.h>

#include <GLFWWindowManager/GLFWWindowManager.h>

#if GLENGINE_GLFW_RENDERER!=GRAPHICS_API_VULKAN
#include <GLRenderer/GLFW/GLFWWindowFactory.h>
#include <GLRenderer/GLFW/OpenGLWindowInfo.h>
#endif

#if CORE_PLATFORM == CORE_PLATFORM_WIN
	#include <DX12Renderer/D3D12WindowFactory.h>
	#include <DX12Renderer/D3D12WindowManager.h>
	#include <DX12Renderer/D3D12WindowInfo.h>
#endif

#if GLENGINE_GLFW_RENDERER==GRAPHICS_API_VULKAN
#include <VulkanRenderer/VkWindowFactory.h>
#include <VulkanRenderer/VkWindowManager.h>
#include <VulkanRenderer/VkWindowInfo.h>
#endif


#include <Utils/Logging/Logging.h>
#include <Utils/Logging/ILogger.h>
#include <Utils/Logging/LoggingMacros.h>

#include <Core/Application.h>
#include <Core/WindowManager.h>

#include <Core/EntryPoint.h>

#include <type_traits>
#include <filesystem>


class SandboxApp : public GLEngine::Core::C_Application {
public:

	//=================================================================================
	virtual void Init() override
	{
		InitInt();

		OpenWindwos();
	}

	//=================================================================================
#if CORE_PLATFORM == CORE_PLATFORM_WIN
	virtual void Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) override
	{
		InitInt();

		auto* manager = GLEngine::DX12Renderer::ConstructD3D12Manager(std::bind(&C_Application::OnEvent, this, std::placeholders::_1), hInstance);
		manager->AddWindowFactory(GLEngine::DX12Renderer::ConstructD3D12WindowFactory());
		m_WndMgr->AddManager<GLEngine::DX12Renderer::C_D3D12WindowManager>(manager);


		OpenWindwos();
	}
#endif

	//=================================================================================
	[[nodiscard]] virtual GLEngine::Renderer::I_Renderer* GetActiveRendererPtr() override
	{
	  return m_WndMgr->ActiveRendererPtr();
	}

	//=================================================================================
	virtual GLEngine::Renderer::I_Renderer& GetActiveRenderer() override
	{
		return m_WndMgr->GetActiveRenderer();
	}
protected:

	void InitInt()
	{
		auto& logging = Utils::Logging::C_LoggingSystem::Instance();
		logging.AddLogger(new Utils::Logging::C_CoutLogger());
		logging.AddLogger(new Utils::Logging::C_FileLogger(std::filesystem::path("log.txt")));

		m_WndMgr = new std::remove_pointer_t<decltype(m_WndMgr)>(std::bind(&C_Application::OnEvent, this, std::placeholders::_1));
#if GLENGINE_GLFW_RENDERER!=GRAPHICS_API_VULKAN
		{
			// we can open glfw window from here
			using namespace GLEngine::GLRenderer;
			auto* manager = GLFW::ConstructGLGLFWManager(std::bind(&C_Application::OnEvent, this, std::placeholders::_1));
			manager->AddWindowFactory(GLFW::ConstructGLFWWindowFactory());
			m_WndMgr->AddManager<GLEngine::GLFWManager::C_GLFWWindowManager>(manager);
		}
#else
		{
			using namespace GLEngine::VkRenderer;
			auto* manager = ConstructVkWManager(std::bind(&C_Application::OnEvent, this, std::placeholders::_1));
			manager->AddWindowFactory(ConstructVkWindowFactory());
			m_WndMgr->AddManager<GLEngine::GLFWManager::C_GLFWWindowManager>(manager);
		}
#endif
	}

	void OpenWindwos()
	{
#if CORE_PLATFORM == CORE_PLATFORM_WIN
		if(false){
			using namespace GLEngine::DX12Renderer;

			S_D3D12WindowInfo info(640, 480);
			info.m_name = "DX Window";
			info.m_WindowClass = "D3D12";

			m_WndMgr->OpenNewWindow(info);
		}
#endif

#if GLENGINE_GLFW_RENDERER!=GRAPHICS_API_VULKAN
		{
			using namespace GLEngine::GLRenderer::GLFW;

			S_OpenGLWindowInfo info(640, 480);
			info.m_name = "ExperimentWindow";
			info.m_WindowClass = "ExperimentWindow";
			info.m_MinorVersion = 4;
			info.m_Maximize = true;

			m_WndMgr->OpenNewWindow(info);
		}
#else

		{
			using namespace GLEngine::VkRenderer;

			S_VkWindowInfo info(640, 480);
			info.m_name = "VkWindow";
			info.m_WindowClass = "VkWindow";
			info.m_Maximize = true;

			m_WndMgr->OpenNewWindow(info);
		}
#endif
	}

	//=================================================================================
	virtual GLEngine::Core::I_WindowManager& GetWndMgr() const override
	{
		return *m_WndMgr;
	}
private:
#if CORE_PLATFORM == CORE_PLATFORM_WIN
	GLEngine::Core::C_WindowManager<GLEngine::GLFWManager::C_GLFWWindowManager, GLEngine::DX12Renderer::C_D3D12WindowManager>* m_WndMgr;
#else
	GLEngine::Core::C_WindowManager<GLEngine::GLFWManager::C_GLFWWindowManager>* m_WndMgr;
#endif
};

GLEngine::Core::C_Application* GLEngine::Core::CreateApplication() {
	return new SandboxApp();
}