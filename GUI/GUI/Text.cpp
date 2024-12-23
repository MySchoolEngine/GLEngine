#include <GUIStdafx.h>

#include <GUI/Text.h>

namespace GLEngine::GUI {

//=================================================================================
C_FormatedText::C_FormatedText(const std::string& formatText)
	: m_FormatText(formatText)
	, m_PrintString(formatText)
{
}

//=================================================================================
bool C_FormatedText::Draw() const
{
	::ImGui::Text("%s", m_PrintString.c_str());
	return false;
}

//=================================================================================
C_FormatedText::C_FormatedText(const C_FormatedText&& other) noexcept
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

} // namespace GLEngine::GUI
