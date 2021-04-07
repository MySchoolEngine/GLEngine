#include <DX12RendererStdafx.h>

#include <DX12Renderer/D3D12WindowManager.h>

#include <DX12Renderer/D3D12Window.h>
#include <DX12Renderer/D3D12WindowFactory.h>
#include <DX12Renderer/D3D12WindowInfo.h>

#include <Renderer/IRenderer.h>

namespace GLEngine::DX12Renderer {

//=================================================================================
C_D3D12WindowManager::C_D3D12WindowManager(Core::C_Application::EventCallbackFn eventCallback, HINSTANCE hInstance)
	: Core::I_WindowManager(eventCallback)
	, m_hInstance(hInstance)
	, m_UpdatingWindow(nullptr)
{
	Init();
}

//=================================================================================
std::shared_ptr<Core::I_Window> C_D3D12WindowManager::OpenNewWindow(const Core::S_WindowInfo& info)
{
	if (info.GetDriver() != Core::E_Driver::DirectX12)
	{
		return nullptr;
	}
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

	std::for_each(m_Windows.begin(), m_Windows.end(), 
		[&](const decltype(m_Windows)::value_type& window) 
		{
			m_UpdatingWindow = window;
			window->Update();
			m_UpdatingWindow = nullptr;
		}
	);
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
		m_UpdatingWindow = window;
		window->OnEvent(event);
		m_UpdatingWindow = nullptr;
	}
}

//=================================================================================
void C_D3D12WindowManager::Init()
{
}

//=================================================================================
Renderer::I_Renderer& C_D3D12WindowManager::GetActiveRenderer()
{
	GLE_ASSERT(m_UpdatingWindow, "Getting renderer outside of update!");
	return m_UpdatingWindow->GetRenderer();
}

//=================================================================================
Renderer::I_Renderer* C_D3D12WindowManager::ActiveRendererPtr()
{
  if (!m_UpdatingWindow)
	return nullptr;
  return &m_UpdatingWindow->GetRenderer();
}

//=================================================================================
// ConstructD3D12Manager
//=================================================================================
C_D3D12WindowManager* ConstructD3D12Manager(Core::C_Application::EventCallbackFn eventCallback, HINSTANCE hInstance)
{
	return new C_D3D12WindowManager(eventCallback, hInstance);
}

}