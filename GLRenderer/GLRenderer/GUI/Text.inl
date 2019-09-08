#pragma once


#include <fmt/format.h>

namespace GLEngine::GLRenderer::GUI {

//=================================================================================
template<class ...Args>
C_Text::C_Text(const char* fmt, Args&&... args)
{
	SetText(fmt, std::forward<Args>(args)...);
}

//=================================================================================
template<class ...Args>
void C_Text::SetText(const char* fmt, Args&&... args)
{
	m_Text = fmt::format(format, std::forward<Args>(args)...);
}
}