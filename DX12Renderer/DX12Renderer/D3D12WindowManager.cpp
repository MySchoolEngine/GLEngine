#include <DX12RendererStdafx.h>

#include <DX12Renderer/D3D12WindowManager.h>

#include <DX12Renderer/D3D12Window.h>
#include <DX12Renderer/D3D12WindowFactory.h>
#include <DX12Renderer/D3D12WindowInfo.h>

namespace GLEngine::DX12Renderer {

//=================================================================================
C_D3D12WindowManager::C_D3D12WindowManager(Core::C_Application::EventCallbackFn eventCallback, HINSTANCE hInstance)
	: Core::I_WindowManager(eventCallback)
	, m_hInstance(hInstance)
{
	Init();
}

//=================================================================================
std::shared_ptr<Core::I_Window> C_D3D12WindowManager::OpenNewWindow(const Core::S_WindowInfo& info)
{
	(dynamic_cast<S_D3D12WindowInfo*>(&const_cast<Core::S_WindowInfo&>(info)))->m_HInstance = m_hInstance;
	auto window = ConstructWindow(info);
	window->SetEventCallback(m_EventCallback);
	m_Windows.push_back(window);
	return window;
}

//=================================================================================
std::shared_ptr<GLEngine::Core::I_Window> C_D3D12WindowManager::GetWindow(GUID guid) const
{
	auto it = std::find_if(m_Windows.begin(), m_Windows.end(), [&guid](const std::shared_ptr<GLEngine::Core::I_Window>& wnd) {
		return wnd->GetGUID() == guid;
	});

	return *it;
}

//=================================================================================
void C_D3D12WindowManager::Update()
{
	m_Windows.erase(std::remove_if(m_Windows.begin(), m_Windows.end(), [](const decltype(m_Windows)::value_type window) {
		return window->WantClose();
	}), m_Windows.end());

	std::for_each(m_Windows.begin(), m_Windows.end(), [](const decltype(m_Windows)::value_type window) {window->Update(); });
}

//=================================================================================
unsigned int C_D3D12WindowManager::NumWindows() const
{
	return static_cast<unsigned int>(m_Windows.size());
}

//=================================================================================
void C_D3D12WindowManager::OnEvent(Core::I_Event& event)
{
	for (auto& window : m_Windows)
	{
		window->OnEvent(event);
	}
}

//=================================================================================
void C_D3D12WindowManager::Init()
{
}

//=================================================================================
API_EXPORT Core::I_WindowManager* ConstructD3D12Manager(Core::C_Application::EventCallbackFn eventCallback, HINSTANCE hInstance)
{
	return new C_D3D12WindowManager(eventCallback, hInstance);
}

}