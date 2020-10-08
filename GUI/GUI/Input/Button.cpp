#include <GUIStdafx.h>

#include <GUI/Input/Button.h>

namespace GLEngine::GUI::Input {

//=================================================================================
C_Button::C_Button(std::string&& name, const std::function<void()>& callback)
	: m_name(std::move(name))
	, m_Callback(callback)
{

}

//=================================================================================
void C_Button::Draw() const
{
	if (::ImGui::Button(m_name.c_str())) {
		m_Callback();
	}
}

}