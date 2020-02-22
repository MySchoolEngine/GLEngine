#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/Menu/Menu.h>


namespace GLEngine::GLRenderer::GUI::Menu {
//=================================================================================
C_Menu::C_Menu(const std::string& label)
	: m_Label(label)
{

}

//=================================================================================
void C_Menu::Draw() const
{
	if (::ImGui::BeginMenu(m_Label.c_str()))
	{
		for (const auto& item : m_MenuItems)
		{
			item.second.get().Draw();
		}
		::ImGui::EndMenu();
	}
}

//=================================================================================
GUID C_Menu::AddMenuItem(T_GUIMenuItem menuItem)
{
	GUID guid = NextGUID();
	m_MenuItems.emplace(guid, menuItem);
	return guid;
}

}