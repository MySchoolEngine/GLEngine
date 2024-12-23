#include <GUIStdafx.h>

#include <GUI/GUIManager.h>
#include <GUI/GUIWindow.h>
#include <GUI/ReflectionGUI.h>

namespace GLEngine::GUI {

//=================================================================================
C_GUIManager::C_GUIManager()
{
	SetGUIManager(*this);
}

//=================================================================================
C_GUIManager::~C_GUIManager()
{
	GLE_ASSERT(CanBeDestroyed(), "Destroying unprepared windows.");
	DestroyPossibleWindows();
	GLE_ASSERT(m_Windows.empty(), "Undestroyed windows.");
}

//=================================================================================
void C_GUIManager::OnUpdate()
{
	for (auto* window : m_Windows | std::views::values)
	{
		window->Update();
	}
	DestroyPossibleWindows();

	for (auto* window : m_Windows | std::views::values)
	{
		if (window->IsVisible())
		{
			window->Draw(*this);
		}
	}
}

//=================================================================================
GUID C_GUIManager::CreateGUIWindow(const std::string& name)
{
	const GUID guid		= NextGUID();
	m_Windows[guid] = new C_Window(guid, name);
	return guid;
}

//=================================================================================
C_Window* C_GUIManager::GetWindow(const GUID guid) const
{
	if (const auto it = m_Windows.find(guid); it != m_Windows.end())
	{
		return (it->second);
	}
	return nullptr;
}

//=================================================================================
void C_GUIManager::DestroyWindow(const GUID guid)
{
	const auto it = m_Windows.find(guid);
	if (it == m_Windows.end())
	{
		return;
	}

	it->second->RequestDestroy();
}

//=================================================================================
void C_GUIManager::AddCustomWindow(C_Window* window)
{
	m_Windows[window->GetGuid()] = window;
}

//=================================================================================
bool C_GUIManager::CanBeDestroyed() const
{
	return std::ranges::all_of(m_Windows, [](const auto& window) -> bool { return window.second->WantDestroy(); });
}

//=================================================================================
void C_GUIManager::RequestDestroy()
{
	std::ranges::for_each(m_Windows, [](const auto& window) { window.second->RequestDestroy(); });
}

//=================================================================================
void C_GUIManager::DestroyPossibleWindows()
{
	std::vector<GUID> toDelete(5);
	std::ranges::for_each(m_Windows, [&](const auto& window) {
		auto* windowPtr = window.second;
		if (windowPtr->WantDestroy() && windowPtr->CanDestroy())
		{
			delete windowPtr;
			toDelete.push_back(window.first);
		}
	});
	std::ranges::for_each(toDelete, [&](const auto& guid) { m_Windows.erase(guid); });
}

} // namespace GLEngine::GUI