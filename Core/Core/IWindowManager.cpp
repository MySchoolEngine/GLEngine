#include <CoreStdafx.h>

#include <Core/IWindowManager.h>

namespace GLEngine::Core {

//=================================================================================
I_WindowManager::I_WindowManager(const C_Application::EventCallbackFn& callback)
	: C_Layer("WindowManager")
	, m_EventCallback(callback)
	, m_Factories(new std::remove_pointer_t<decltype(m_Factories)>)
{
}

//=================================================================================
I_WindowManager::~I_WindowManager()
{
	for (auto* wf : *m_Factories)
	{
		delete wf;
	}
	delete m_Factories;
}

//=================================================================================
void I_WindowManager::AddWindowFactory(I_WindowFactory* wf)
{
	m_Factories->emplace_back(std::move(wf));
}

//=================================================================================
std::shared_ptr<I_Window> I_WindowManager::ConstructWindow(const S_WindowInfo& info) const
{
	for (const auto* wf : *m_Factories)
	{
		auto window = wf->GetWindow(info);
		if (window)
		{
			return window;
		}
	}
	return nullptr;
}

} // namespace GLEngine::Core