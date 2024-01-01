#include <GUIStdafx.h>

#include <GUI/Input/Button.h>

namespace GLEngine::GUI::Input {

//=================================================================================
C_Button::C_Button(std::string&& name, const std::function<bool()>& callback)
	: m_name(std::move(name))
	, m_Callback(callback)
{
}

//=================================================================================
bool C_Button::Draw() const
{
	if (::ImGui::Button(m_name.c_str()))
	{
		return m_Callback();
	}
	return false;
}

} // namespace GLEngine::GUI::Input