#include <GUIStdafx.h>

#include <GUI/Menu/MenuItem.h>
#include <GUI/GUIWindow.h>

#include <GUI/GUIManager.h>

namespace GLEngine::GUI::Menu {

//=================================================================================
C_MenuItem::C_MenuItem(const std::string& label, const std::function<void()>& callback, const std::string& shortcut)
	: m_Label(label)
	, m_Callback(callback)
	, m_Shortcut(shortcut)
{

}

//=================================================================================
void C_MenuItem::Draw() const
{
	if (::ImGui::MenuItem(m_Label.c_str(), m_Shortcut.c_str()))
	{ 
		m_Callback();
	}
}

//=================================================================================
const auto openWindow = [](GUID id, const C_GUIManager& guiMGR) {
	if (auto* window = guiMGR.GetWindow(id))
	{
		window->SetVisible(true);
	}
};

//=================================================================================
C_MenuItemOpenWindow::C_MenuItemOpenWindow(const std::string& label, GUID window, const C_GUIManager& guiMGR, const std::string& shortcut /*= ""*/) : C_MenuItem(label, std::bind(openWindow, window, std::cref(guiMGR)), shortcut)
{

}

}
