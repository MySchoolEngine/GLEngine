#include <EntityStdafx.h>

#include <Entity/BasicEntity.h>
#include <Entity/EntitiesWindow.h>
#include <Entity/EntityManager.h>

#include <imgui.h>

namespace GLEngine::Entity {

//=================================================================================
C_EntitiesWindow::C_EntitiesWindow(GUID guid, std::shared_ptr<C_EntityManager>& world)
	: GUI::C_Window(guid, "Entities")
	, m_World(world)
	, m_SelectedEntity(GUID::INVALID_GUID)
{
}

//=================================================================================
C_EntitiesWindow::~C_EntitiesWindow() = default;

//=================================================================================
void C_EntitiesWindow::Draw() const
{
	::ImGui::Begin(m_Name.c_str(), &m_IsVisible);
	for (const auto& entity : m_World->GetEntities())
	{
		bool selected = false;
		::ImGui::Selectable(entity->GetName().c_str(), &selected);
		if (selected)
		{
			Core::C_EntityEvent event(entity->GetID(), Core::C_EntityEvent::EntityEvent::Seleced);
			//TODO: I need to sand this to the event tree root
			entity->OnEvent(event);
			m_SelectedEntity = entity->GetID();
		}
	}

	auto entity = m_World->GetEntity(m_SelectedEntity);
	if (entity)
	{
		ImGui::BeginChild("SelectedEntity");
		int i = 0;
		for (auto& component : *entity)
		{
			ImGui::PushID(i);
			if (component.second->HasDebugDrawGUI())
				component.second->DebugDrawComponentGUI();
			ImGui::PopID();
			++i;
		}
		ImGui::EndChild();
	}
	::ImGui::End();
}

} // namespace GLEngine::Entity