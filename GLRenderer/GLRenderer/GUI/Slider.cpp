#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/Slider.h>


namespace GLEngine {
namespace GLRenderer {
namespace GUI {


//=================================================================================
template<>
void C_Slider<int>::Draw() const
{
	m_Prev = m_Value;
	::ImGui::SliderInt(m_Name.c_str(), &m_Value, m_Min, m_Max);
}

//=================================================================================
template<>
void C_Slider<float>::Draw() const
{
	m_Prev = m_Value;
	::ImGui::SliderFloat(m_Name.c_str(), &m_Value, m_Min, m_Max);
}

}}}