#include <GUIStdafx.h>

#include <GUI/Input/Color.h>


RTTR_REGISTRATION
{
	using namespace GLEngine::GUI::Input;

	rttr::registration::class_<C_ColorRBG>("ColorRBG")
		.constructor<std::string&&, GLEngine::Colours::T_Colour>()
		.property("Colour", &C_ColorRBG::m_Color);
	
	rttr::registration::class_<C_ColorRBGA>("ColorRBGA")
		.constructor<std::string&&, glm::vec4>()
		.property("Colour", &C_ColorRBGA::m_Color);
}

namespace GLEngine::GUI::Input {

//=================================================================================
// C_ColorRBG
//=================================================================================
bool C_ColorRBG::Draw() const
{
	float* col;
	col = &(m_Color.x);
	return ::ImGui::ColorEdit3(m_name.c_str(), col);
}

//=================================================================================
// C_ColorRBGA
//=================================================================================
bool C_ColorRBGA::Draw() const
{
	float* col;
	col = &(m_Color.x);
	return ::ImGui::ColorEdit4(m_name.c_str(), col);
}

} // namespace GLEngine::GUI::Input