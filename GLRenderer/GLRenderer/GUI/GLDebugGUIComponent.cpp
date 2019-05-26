#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/GLDebugGUIComponent.h>

namespace GLEngine {
namespace GLRenderer {
namespace GUI {


//=================================================================================
C_GLDebugGUIComponent::C_GLDebugGUIComponent(const std::string& name)
	: m_Title(name)
	, m_Show(false)
{

}

//=================================================================================
void C_GLDebugGUIComponent::Toggle()
{
	m_Show = !m_Show;
}

//=================================================================================
void C_GLDebugGUIComponent::PostUpdate()
{
	if (m_Show) {
		::ImGui::Begin(m_Title.c_str(), &m_Show);
		DrawContents();
		::ImGui::End();
	}
}

}}}