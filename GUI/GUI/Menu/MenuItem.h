#pragma once

#include <GUI/GUIPart.h>

namespace GLEngine::GUI {
class C_GUIManager;
}

namespace GLEngine::GUI::Menu {

class GUI_API_EXPORT C_MenuItem : public I_GUIPart {
public:
	/**
	 * The callback should return true if the edited value have been changed.
	 */
	C_MenuItem(const std::string& label, const std::function<bool()>& callback, const std::string& shortcut = "");

	bool Draw() const override;

private:
	std::string			  m_Label;
	std::string			  m_Shortcut;
	std::function<bool()> m_Callback;
};

class GUI_API_EXPORT C_MenuItemOpenWindow final : public C_MenuItem {
public:
	C_MenuItemOpenWindow(const std::string& label, GUID window, const C_GUIManager& guiMGR, const std::string& shortcut = "");
};

} // namespace GLEngine::GUI::Menu