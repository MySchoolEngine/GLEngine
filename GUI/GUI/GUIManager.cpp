#include <GUIStdafx.h>

#include <GUI/GUIManager.h>
#include <GUI/GUIWindow.h>

namespace GLEngine::GUI {

//=================================================================================
C_GUIManager::C_GUIManager() = default;

//=================================================================================
C_GUIManager::~C_GUIManager()
{
	GLE_ASSERT(CanBeDestroyed(), "Destroying unprepared windows.");
	DestroyPossibleWindows();
	GLE_ASSERT(m_Windwos.empty(), "Undestroyed windows.");
}

//=================================================================================
void C_GUIManager::OnUpdate()
{
	for (auto& it : m_Windwos)
	{
		it.second->Update();
	}

	for (auto& it : m_Windwos)
	{
		if (it.second->IsVisible())
		{
			it.second->Draw();
		}
	}

	DestroyPossibleWindows();
}

//=================================================================================
GUID C_GUIManager::CreateGUIWindow(const std::string& name)
{
	GUID guid		= NextGUID();
	m_Windwos[guid] = new GUI::C_Window(guid, name);
	return guid;
}

//=================================================================================
GUI::C_Window* C_GUIManager::GetWindow(GUID guid) const
{
	auto it = m_Windwos.find(guid);
	if (it != m_Windwos.end())
	{
		return (it->second);
	}
	return nullptr;
}

//=================================================================================
void C_GUIManager::DestroyWindow(GUID guid)
{
	auto it = m_Windwos.find(guid);
	if (it == m_Windwos.end())
	{
		return;
	}

	it->second->RequestDestroy();
}

//=================================================================================
void C_GUIManager::AddCustomWindow(GUI::C_Window* window)
{
	m_Windwos[window->GetGuid()] = window;
}

//=================================================================================
bool C_GUIManager::CanBeDestroyed() const
{
	return std::all_of(m_Windwos.begin(), m_Windwos.end(), [](const auto& window) -> bool { return window.second->WantDestroy(); });
}

//=================================================================================
void C_GUIManager::RequestDestroy()
{
	std::for_each(m_Windwos.begin(), m_Windwos.end(), [](const auto& window) { window.second->RequestDestroy(); });
}

//=================================================================================
void C_GUIManager::DestroyPossibleWindows()
{
	std::vector<GUID> toDelete(5);
	std::for_each(m_Windwos.begin(), m_Windwos.end(), [&](const auto& window) {
		auto* windowPtr = window.second;
		if (windowPtr->WantDestroy() && windowPtr->CanDestroy())
		{
			delete windowPtr;
			toDelete.push_back(window.first);
		}
	});
	std::for_each(toDelete.begin(), toDelete.end(), [&](const auto& guid) { m_Windwos.erase(guid); });
}

} // namespace GLEngine::GUI