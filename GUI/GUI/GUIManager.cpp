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
	for (auto& it : m_Windwos)
	{
		delete (it.second);
	}
}

//=================================================================================
void C_GUIManager::OnUpdate()
{
	for (auto& it : m_Windwos)
	{
		if (it.second->IsVisible())
		{
			it.second->Draw();
		}
	}
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

	delete it->second;

	m_Windwos.erase(guid);
}

//=================================================================================
void C_GUIManager::AddCustomWindow(GUI::C_Window* window)
{
	m_Windwos[window->GetGuid()] = window;
}

//=================================================================================
bool C_GUIManager::CanBeDestroyed() const
{
	return std::all_of(m_Windwos.begin(), m_Windwos.end(), [](const auto& window) -> bool { return window.second->CanDestroy(); });
}

//=================================================================================
void C_GUIManager::RequestDestroy()
{
	std::for_each(m_Windwos.begin(), m_Windwos.end(), [](const auto& window) { window.second->RequestDestroy(); });
}

} // namespace GLEngine::GUI