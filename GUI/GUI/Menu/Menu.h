#pragma once

#include <GUI/GUIPart.h>
#include <GUI/Menu/MenuItem.h>

namespace GLEngine::GUI::Menu {

class GUI_API_EXPORT C_Menu : public I_GUIPart {
public:
	using T_GUIMenuItem = std::reference_wrapper<Menu::C_MenuItem>;

public:
	explicit C_Menu(const std::string&& label);

	virtual void Draw() const override;

	GUID AddMenuItem(T_GUIMenuItem menuItem);

private:
	std::string					  m_Label;
	std::map<GUID, T_GUIMenuItem> m_MenuItems;
};

} // namespace GLEngine::GUI::Menu