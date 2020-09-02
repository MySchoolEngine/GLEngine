#pragma once

#include <GUI/GUIPart.h>

namespace GLEngine::GUI {
class C_GUIManager;
}

namespace GLEngine::GUI::Menu {

class GUI_API_EXPORT C_MenuItem : public I_GUIPart {
public:
	C_MenuItem(const std::string& label, const std::function<void()>& callback, const std::string& shortcut = "");

	virtual void Draw() const override;

private:

#pragma warning(push)
#pragma warning( disable : 4251)
	std::string						m_Label;
	std::string						m_Shortcut;
	std::function<void()> m_Callback;
#pragma warning(pop)
};

class C_MenuItemOpenWindow : public C_MenuItem
{
public:
	C_MenuItemOpenWindow(const std::string& label, GUID window, const C_GUIManager& guiMGR, const std::string& shortcut = "");
};

}