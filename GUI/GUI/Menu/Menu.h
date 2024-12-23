#pragma once

#include <GUIStdafx.h>

#include <GUI/GUIPart.h>
#include <GUI/Menu/MenuItem.h>

namespace GLEngine::GUI::Menu {

class GUI_API_EXPORT C_Menu : public I_GUIPart {
public:
	using T_GUIMenuItem = std::reference_wrapper<C_MenuItem>;

	explicit C_Menu(std::string&& label);
	~C_Menu() override;

	/**
	 *
	 * @return true if any item have been called
	 */
	bool Draw() const override;

	virtual GUID AddMenuItem(std::reference_wrapper<C_MenuItem> menuItem);

private:
	std::string									m_Label;
	std::vector<std::pair<GUID, T_GUIMenuItem>> m_MenuItems;
};

} // namespace GLEngine::GUI::Menu