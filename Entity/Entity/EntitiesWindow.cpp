#include <EntityStdafx.h>

#include <Entity/BasicEntity.h>
#include <Entity/Components/DebugGUIComponent.h>
#include <Entity/EntitiesWindow.h>
#include <Entity/EntityManager.h>

#include <imgui.h>

namespace GLEngine::Entity {

//=================================================================================
C_EntitiesWindow::C_EntitiesWindow(GUID guid, std::shared_ptr<C_EntityManager>& world, GUI::C_GUIManager& guiMGR)
	: GUI::C_Window(guid, "Entities")
	, m_World(world)
	, m_guiMGR(guiMGR)
{
}

//=================================================================================
C_EntitiesWindow::~C_EntitiesWindow() = default;

//=================================================================================
void C_EntitiesWindow::Draw() const
{
	::ImGui::Begin(m_Name.c_str(), &m_IsVisible);
	for (auto& entity : m_World->GetEntities())
	{
		bool selected = false;
		::ImGui::Selectable(entity->GetName().c_str(), &selected);
		if (selected)
		{
			m_SelectedEntity	= entity;
		}
	}
	if (auto selectedEntity = m_SelectedEntity.lock())
	{
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 300);
		if (ImGui::BeginChild("Selected entity", ImVec2(0, 290)))
		{
			ImGui::Separator();
			ImGui::Text("Details: %s", selectedEntity->GetName().c_str());
			for (auto& component : *selectedEntity)
			{
				if (component.second->HasDebugDrawGUI())
					component.second->DebugDrawComponentGUI(&m_guiMGR);
			}
		}
		ImGui::EndChild();
	}

	::ImGui::End();
}

} // namespace GLEngine::Entity