#include <EntityStdafx.h>

#include <Entity/BasicEntity.h>
#include <Entity/EntitiesWindow.h>
#include <Entity/EntityManager.h>

#include <GUI/GUIUtils.h>

#include <imgui.h>

namespace GLEngine::Entity {

//=================================================================================
C_EntitiesWindow::C_EntitiesWindow(GUID guid, std::shared_ptr<C_EntityManager>& world)
	: GUI::C_Window(guid, "Entities")
	, m_World(world)
	, m_SelectedEntity(GUID::INVALID_GUID)
	, m_EntityTypeSelector("Entity class", rttr::type::get<I_Entity>().get_name().data())
{
}

//=================================================================================
C_EntitiesWindow::~C_EntitiesWindow() = default;

//=================================================================================
void C_EntitiesWindow::Draw() const
{
	ImGui::Begin(m_Name.c_str(), &m_IsVisible);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	static float SceneTreeHeight = 300.0f;
	auto		 world			 = m_World.lock();
	if (world)
	{
		ImGui::BeginChild("SceneTree", ImVec2(0, std::max(50.f, SceneTreeHeight)), true);

		for (const auto& entity : world->GetEntities())
		{
			bool selected = false;
			ImGui::Selectable(entity->GetName().c_str(), &selected);
			if (selected)
			{
				Core::C_EntityEvent event(entity->GetID(), Core::C_EntityEvent::EntityEvent::Seleced);
				// TODO: I need to sand this to the event tree root
				entity->OnEvent(event);
				if (m_SelectedEntity == entity->GetID())
					m_SelectedEntity = GUID::INVALID_GUID;
				else
					m_SelectedEntity = entity->GetID();
			}
		}
		ImGui::EndChild();
	}

	SceneTreeHeight += GUI::Splitter();

	ImGui::BeginChild("SpawnEntity", ImVec2(0, 150), true);
	ImGui::PopStyleVar();

	m_EntityTypeSelector.Draw();
	if (ImGui::Button("Spawn")) {
		const auto type = rttr::type::get_by_name(m_EntityTypeSelector.GetSelectedTypeName());
		if (type.is_valid() == false) {
			CORE_LOG(E_Level::Error, E_Context::Entity, "Type {} doesn't exists.", m_EntityTypeSelector.GetSelectedTypeName());
		}
		else
		{
			// auto entity = type.create();
			// m_World->AddEntity(entity.convert(rttr::type::get<>());
		}
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::EndChild();


	// here should be way how to add components
	if (world)
	{
		auto entity = world->GetEntity(m_SelectedEntity);
		if (entity)
		{
			ImGui::BeginChild("SelectedEntity", ImVec2(0, 0), true);
			ImGui::PopStyleVar();
			ImGui::Text("%s", entity->GetName().c_str());
			int i = 0;
			for (auto& component : *entity)
			{
				ImGui::PushID(i);
				if (component.second->HasDebugDrawGUI())
					component.second->DebugDrawComponentGUI();
				ImGui::PopID();
				++i;
			}
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::EndChild();
		}
	}

	if (!world)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::Text("World went missing!");
		ImGui::PopStyleColor();
	}

	ImGui::PopStyleVar();
	ImGui::End();
}

//=================================================================================
void C_EntitiesWindow::SetWorld(std::shared_ptr<C_EntityManager> world)
{
	m_World = world;
	// entity removed anyway
	m_SelectedEntity=GUID::INVALID_GUID;
}

} // namespace GLEngine::Entity