#include <GUIStdafx.h>

#include <GUI/DebugGUIComponent.h>

namespace GLEngine::GUI {

//=================================================================================
C_DebugGUIComponent::C_DebugGUIComponent(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_DebugGUIComponent(owner)
	, m_Title(owner->GetName())
	, m_Show(false)
{

}

//=================================================================================
void C_DebugGUIComponent::Toggle()
{
	m_Show = !m_Show;
}

//=================================================================================
void C_DebugGUIComponent::PostUpdate()
{
	if (m_Show) {
		::ImGui::Begin(m_Title.c_str(), &m_Show);
			DrawContents();
		::ImGui::End();
	}
}

//=================================================================================
void C_DebugGUIComponent::SetTitle(const std::string& title)
{
	m_Title = title;
}

}