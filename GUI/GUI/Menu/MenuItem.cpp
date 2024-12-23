#include <GUIStdafx.h>

#include <GUI/GUIManager.h>
#include <GUI/GUIWindow.h>
#include <GUI/Menu/MenuItem.h>

namespace GLEngine::GUI::Menu {

//=================================================================================
C_MenuItem::C_MenuItem(const std::string& label, const std::function<bool()>& callback, const std::string& shortcut)
	: m_Label(label)
	, m_Shortcut(shortcut)
	, m_Callback(callback)
{
}

//=================================================================================
bool C_MenuItem::Draw() const
{
	if (::ImGui::MenuItem(m_Label.c_str(), m_Shortcut.c_str()))
	{
		return m_Callback();
	}
	return false;
}

//=================================================================================
const auto s_OpenWindow = [](GUID id, const C_GUIManager& guiMGR) {
	if (auto* window = guiMGR.GetWindow(id))
	{
		window->SetVisible(true);
	}
	return false;
};

//=================================================================================
C_MenuItemOpenWindow::C_MenuItemOpenWindow(const std::string& label, GUID window, const C_GUIManager& guiMGR, const std::string& shortcut /*= ""*/)
	: C_MenuItem(label, std::bind(s_OpenWindow, window, std::cref(guiMGR)), shortcut)
{
}

} // namespace GLEngine::GUI::Menu
