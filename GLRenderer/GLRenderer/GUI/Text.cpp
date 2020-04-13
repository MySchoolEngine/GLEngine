#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/Text.h>

namespace GLEngine::GLRenderer::GUI {

//=================================================================================
C_FormatedText::C_FormatedText(std::string formatText)
	: m_FormatText(formatText)
	, m_PrintString(formatText)
{

}

//=================================================================================
void C_FormatedText::Draw() const
{
	::ImGui::Text(m_PrintString.c_str());
}

//=================================================================================
C_FormatedText::C_FormatedText(const C_FormatedText&& other)
	: m_FormatText(std::move(other.m_FormatText))
	, m_PrintString(std::move(other.m_PrintString))
{

}

//=================================================================================
C_FormatedText::C_FormatedText(const C_FormatedText& other)
	: m_FormatText(other.m_FormatText)
	, m_PrintString(other.m_PrintString)
{

}

//=================================================================================
const std::string_view C_FormatedText::GetCurrentText() const
{
	return m_PrintString;
}

}
