#pragma once

#include <GUIStdafx.h>

#include <fmt/format.h>


namespace GLEngine::GUI {

//=================================================================================
template <class... Args> void C_FormatedText::UpdateText(Args&&... args)
{
	m_PrintString = fmt::format(m_FormatText, std::forward<Args>(args)...);
}


} // namespace GLEngine::GUI
