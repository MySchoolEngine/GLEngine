#pragma once

#include <GLRenderer/GUI/GUIPart.h>
#include <GLRenderer/GUI/Menu/MenuItem.h>

namespace GLEngine::GLRenderer::GUI::Menu {

class C_Menu : public I_GUIPart {
public:
	using T_GUIMenuItem = std::reference_wrapper<Menu::C_MenuItem>;
public:
	C_Menu(const std::string& label);

	virtual void Draw() const override;

	GUID AddMenuItem(T_GUIMenuItem menuItem);

private:
	std::string										m_Label;
	std::map<GUID, T_GUIMenuItem> m_MenuItems;
};

}