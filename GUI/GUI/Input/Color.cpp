#include <GUIStdafx.h>

#include <GUI/Input/Color.h>

namespace GLEngine::GUI::Input {

//=================================================================================
// C_ColorRBG
//=================================================================================
void C_ColorRBG::Draw() const
{
	float *col;
	col = &(m_Color.x);
	::ImGui::ColorEdit3(m_name.c_str(), col);
}

//=================================================================================
// C_ColorRBGA
//=================================================================================
void C_ColorRBGA::Draw() const
{
	float* col;
	col = &(m_Color.x);
	::ImGui::ColorEdit4(m_name.c_str(), col);
}

}