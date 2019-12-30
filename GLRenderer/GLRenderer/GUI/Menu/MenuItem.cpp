#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/Menu/MenuItem.h>

namespace GLEngine::GLRenderer::GUI::Menu {

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
	if (ImGui::MenuItem(m_Label.c_str(), m_Shortcut.c_str()))
	{ 
		m_Callback();
	}
}

}
