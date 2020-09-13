#include <EntityStdafx.h>

#include <Entity/EntitiesWindow.h>

#include <Entity/EntityManager.h>
#include <Entity/BasicEntity.h>

#include <imgui.h>

namespace GLEngine::Entity {

//=================================================================================
C_EntitiesWindow::C_EntitiesWindow(GUID guid, std::shared_ptr<C_EntityManager>& world)
	: GUI::C_Window(guid, "Entities")
	, m_World(world)
{}

//=================================================================================
C_EntitiesWindow::~C_EntitiesWindow() = default;

//=================================================================================
void C_EntitiesWindow::Draw() const
{
	::ImGui::Begin(m_Name.c_str(), &m_IsVisible);
	for (const auto& entity : m_World->GetEntities()) {
		bool selected = false;
		::ImGui::Selectable(entity->GetName().c_str(), &selected);
		if (selected) {
			entity->OnEvent(Core::C_UserEvent("selected"));
		}
	}
	::ImGui::End();
}

}