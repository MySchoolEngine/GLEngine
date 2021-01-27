#include <EntityStdafx.h>

#include <Entity/Components/DebugGUIComponent.h>

#include <imgui.h>

namespace GLEngine::Entity {

//=================================================================================
C_DebugGUIComponent::C_DebugGUIComponent(std::shared_ptr<I_Entity> owner)
	: I_DebugGUIComponent(owner)
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
	if (m_Show)
	{
		::ImGui::Begin(m_Title.c_str(), &m_Show);
		DrawContents();
		::ImGui::End();
	}
}

} // namespace GLEngine::Entity