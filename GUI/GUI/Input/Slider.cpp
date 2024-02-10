#include <GUIStdafx.h>

#include <GUI/Input/Slider.h>


namespace GLEngine::GUI::Input {


//=================================================================================
template <> bool C_Slider<int>::Draw() const
{
	m_Prev = m_Value;
	return ::ImGui::SliderInt(m_Name.c_str(), &m_Value, m_Min, m_Max);
}

//=================================================================================
template <> bool C_Slider<float>::Draw() const
{
	m_Prev = m_Value;
	return ::ImGui::SliderFloat(m_Name.c_str(), &m_Value, m_Min, m_Max);
}

} // namespace GLEngine::GUI::Input