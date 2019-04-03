#include <CoreStdafx.h>

#include <Core/IWindowManager.h>


namespace GLEngine {
namespace Core {

//=================================================================================
I_WindowManager::I_WindowManager(C_Application::EventCallbackFn callback)
	: Core::C_Layer("WindowManager")
	, m_EventCallback(callback)
	, m_Facotries(new std::remove_pointer<decltype(m_Facotries)>::type)
{

}

//=================================================================================
I_WindowManager::~I_WindowManager()
{
	for (auto* wf : *m_Facotries)
	{
		delete wf;
	}
	delete m_Facotries;
}

//=================================================================================
void I_WindowManager::AddWindowFactory(I_WindowFactory* wf)
{
	m_Facotries->emplace_back(std::move(wf));
}

//=================================================================================
std::shared_ptr<Core::I_Window> I_WindowManager::ConstructWindow(const S_WindowInfo& info) const
{
	for (const auto * wf : *m_Facotries) {
		auto window = wf->GetWindow(info);
		if (window) {
			return window;
		}
	}
	return nullptr;
}

}}