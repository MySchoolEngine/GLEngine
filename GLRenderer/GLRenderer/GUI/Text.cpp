#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/Text.h>


namespace GLEngine::GLRenderer::GUI {

//=================================================================================
void C_Text::Draw() const
{
	::ImGui::Text(m_Text.c_str());
}

}