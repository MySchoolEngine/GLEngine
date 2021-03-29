#include <EntityStdafx.h>

#include <Entity/BasicEntity.h>
#include <Entity/EntitiesWindow.h>
#include <Entity/EntityManager.h>
#include <Entity/Components/DebugGUIComponent.h>

#include <imgui.h>

namespace GLEngine::Entity {

//=================================================================================
C_EntitiesWindow::C_EntitiesWindow(GUID guid, std::shared_ptr<C_EntityManager>& world)
	: GUI::C_Window(guid, "Entities")
	, m_World(world)
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
			Core::C_UserEvent event("selected");
			entity->OnEvent(event);
		}
	}
	::ImGui::End();
}

} // namespace GLEngine::Entity