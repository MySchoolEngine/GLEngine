#include <GUIStdafx.h>

#include <GUI/Menu/Menu.h>


namespace GLEngine::GUI::Menu {
//=================================================================================
C_Menu::C_Menu(std::string&& label)
	: m_Label(std::move(label))
{
}

//=================================================================================
C_Menu::~C_Menu() = default;

//=================================================================================
bool C_Menu::Draw() const
{
	bool ret = false;
	if (::ImGui::BeginMenu(m_Label.c_str()))
	{
		for (const auto& menuItem : m_MenuItems | std::views::values)
		{
			ret |= menuItem.get().Draw();
		}
		::ImGui::EndMenu();
	}
	return ret;
}

//=================================================================================
GUID C_Menu::AddMenuItem(std::reference_wrapper<C_MenuItem> menuItem)
{
	GUID guid = NextGUID();
	m_MenuItems.emplace_back(guid, menuItem);
	return guid;
}


} // namespace GLEngine::GUI::Menu