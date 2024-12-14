#include <EditorStdafx.h>

#include <Editor/EntityEditor/EntitiesWindow.h>

#include <GUI/GUIUtils.h>

#include <Entity/BasicEntity.h>
#include <Entity/EntityManager.h>

#include <Core/Application.h>
#include <Core/EventSystem/Event.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace GLEngine::Editor {

//=================================================================================
C_EntitiesWindow::C_EntitiesWindow(GUID guid, std::shared_ptr<Entity::C_EntityManager>& world)
	: GUI::C_Window(guid, "Entities")
	, m_World(world)
	, m_SelectedEntity(GUID::INVALID_GUID)
	, m_EntityTypeSelector("Entity class", rttr::type::get<Entity::I_Entity>().get_name().data())
{
}

//=================================================================================
C_EntitiesWindow::~C_EntitiesWindow() = default;

//=================================================================================
bool C_EntitiesWindow::Draw(GUI::C_GUIManager& guiMgr) const
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
				Core::C_EntityEvent event(entity->GetID(), Core::C_EntityEvent::EntityEvent::Selected);
				Core::C_Application::Get().OnEvent(event);
				if (m_SelectedEntity == entity->GetID())
					m_SelectedEntity = GUID::INVALID_GUID;
				else
					m_SelectedEntity = entity->GetID();
			}
		}
		ImGui::EndChild();
	}

	if (world)
	{
		SceneTreeHeight += GUI::Splitter();

		ImGui::BeginChild("SpawnEntity", ImVec2(0, 150), true);
		ImGui::PopStyleVar();

		m_EntityTypeSelector.Draw();
		static std::string entityName;
		if (ImGui::Button("Spawn") || ImGui::InputText("Entity name", &entityName))
		{
			const auto type = rttr::type::get_by_name(m_EntityTypeSelector.GetSelectedTypeName());
			if (entityName.empty())
			{
				CORE_LOG(E_Level::Error, E_Context::Entity, "Cannot spawn entity without a name.");
			}
			else if (type.is_valid() == false)
			{
				CORE_LOG(E_Level::Error, E_Context::Entity, "Type {} doesn't exists.", m_EntityTypeSelector.GetSelectedTypeName());
			}
			else
			{
				auto entityVar = type.create({entityName});
				auto entity	   = entityVar.convert<std::shared_ptr<Entity::I_Entity>>();
				world->AddEntity(entity);
				m_SelectedEntity = entity->GetID();
				entityName		 = "";
			}
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::EndChild();
		ImGui::BeginChild("SelectedEntity", ImVec2(0, 0), true);
		ImGui::PopStyleVar();


		// here should be way how to add components
		auto entity = world->GetEntity(m_SelectedEntity);
		m_ComponentEditor.SetEntity(entity);
		m_ComponentEditor.Draw();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::EndChild();
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::Text("World went missing!");
		ImGui::PopStyleColor();
	}

	ImGui::PopStyleVar();
	ImGui::End();
	return false;
}

//=================================================================================
void C_EntitiesWindow::SetWorld(std::shared_ptr<Entity::C_EntityManager> world)
{
	m_World = world;
	// entity removed anyway
	m_SelectedEntity = GUID::INVALID_GUID;
}

} // namespace GLEngine::Editor