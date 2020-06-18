#include <GLFWWindowManagerStdafx.h>

#include <GLFWWindowManager/GLFWWindowManager.h>

#include <Renderer/IRenderer.h>

namespace GLEngine::GLFWManager {

//=================================================================================
C_GLFWWindowManager::C_GLFWWindowManager(Core::C_Application::EventCallbackFn eventCallback, Core::E_Driver driver)
	: Core::I_WindowManager(eventCallback)
	, m_UpdatingWindow(nullptr)
	, m_Driver(driver)
{
	Init();
}

//=================================================================================
std::shared_ptr<Core::I_Window> C_GLFWWindowManager::OpenNewWindow(const Core::S_WindowInfo& info)
{
	if (info.GetDriver() != m_Driver)
	{
		return nullptr;
	}
	auto window = ConstructWindow(info);
	window->SetEventCallback(m_EventCallback);
	m_Windows.push_back(window);
	return window;
}

//=================================================================================
const std::unique_ptr<Renderer::I_Renderer>& C_GLFWWindowManager::GetActiveRenderer() const
{
	if (!m_UpdatingWindow)
	{
		return nullptr;
	}
	return m_UpdatingWindow->GetRenderer();
}

//=================================================================================
std::shared_ptr<GLEngine::Core::I_Window> C_GLFWWindowManager::GetWindow(GUID guid) const
{
	auto it = std::find_if(m_Windows.begin(), m_Windows.end(), [&guid](const std::shared_ptr<GLEngine::Core::I_Window>& wnd) {
		return wnd->GetGUID() == guid;
	});

	return *it;
}

//=================================================================================
void C_GLFWWindowManager::Update()
{
	m_Windows.erase(std::remove_if(m_Windows.begin(), m_Windows.end(), [](const decltype(m_Windows)::value_type& window) {
		return window->WantClose();
	}), m_Windows.end());

	std::for_each(m_Windows.begin(), m_Windows.end(), 
		[&](const decltype(m_Windows)::value_type& window) {
			m_UpdatingWindow = window;
			window->Update();
			m_UpdatingWindow = nullptr;
		}
	);

	glfwPollEvents();
}

//=================================================================================
unsigned int C_GLFWWindowManager::NumWindows() const
{
	return static_cast<unsigned int>(m_Windows.size());
}

//=================================================================================
void C_GLFWWindowManager::OnEvent(Core::I_Event& event)
{
	for (auto& window : m_Windows)
	{
		m_UpdatingWindow = window;
		window->OnEvent(event);
		m_UpdatingWindow = nullptr;
	}
}

//=================================================================================
void C_GLFWWindowManager::Init()
{
	const auto error_callback = [](int error, const char* description) {
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: #{}: {}", error, description);
	};
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: Unable to init glfw. Terminating engine");
		exit(EXIT_FAILURE);
	}
}

}