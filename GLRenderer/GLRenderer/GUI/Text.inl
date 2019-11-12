#pragma once

#include <GLRendererStdafx.h>

#include <fmt/format.h>


namespace GLEngine::GLRenderer::GUI {

//=================================================================================
template<class ...Args>
void GLEngine::GLRenderer::GUI::C_FormatedText::UpdateText(Args&&... args)
{
	m_PrintString = fmt::format(m_FormatText, std::forward<Args>(args)...);
}


}
