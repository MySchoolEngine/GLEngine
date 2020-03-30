#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/Input/Color.h>

namespace GLEngine::GLRenderer::GUI::Input {

//=================================================================================
// C_ColorRBG
//=================================================================================
C_ColorRBG::C_ColorRBG(std::string&& name, glm::vec3 default)
	: m_name(std::move(name))
	, m_Color(default)
{

}

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
C_ColorRBGA::C_ColorRBGA(std::string&& name, glm::vec4 default)
	: m_name(std::move(name))
	, m_Color(default)
{

}

//=================================================================================
void C_ColorRBGA::Draw() const
{
	float* col;
	col = &(m_Color.x);
	::ImGui::ColorEdit4(m_name.c_str(), col);
}

}