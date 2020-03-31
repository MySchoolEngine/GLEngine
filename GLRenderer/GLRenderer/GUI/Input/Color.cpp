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
void C_ColorRBG::SetValue(glm::vec3&& value)
{
	m_Color = value;
}

//=================================================================================
const glm::vec3& C_ColorRBG::GetValue() const
{
	return m_Color;
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

//=================================================================================
void C_ColorRBGA::SetValue(glm::vec4&& value)
{
	m_Color = value;
}

//=================================================================================
const glm::vec4& C_ColorRBGA::GetValue() const
{
	return m_Color;
}

}