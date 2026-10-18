#include <EditorStdafx.h>

#include <Editor/EntityEditor/EntitiesWindow.h>

#include <GUI/GUIUtils.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/World.h>

#include <Core/Application.h>
#include <Core/EventSystem/Event.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

namespace GLEngine::Editor {

//=================================================================================
C_EntitiesWindow::C_EntitiesWindow(GUID guid, Entity::C_World& world)
	: GUI::C_Window(guid, "Entities")
	, m_World(&world)
	, m_SelectedEntity(GUID::INVALID_GUID)
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
	ImGui::BeginChild("SceneTree", ImVec2(0, std::max(50.f, SceneTreeHeight)), true);

	auto& registry = m_World->Registry();
	for (auto [e, identity] : registry.view<Entity::S_IdentityComponent>().each())
	{
		const auto entityGUID = identity.id;
		const bool selected	  = entityGUID == m_SelectedEntity;
		if (ImGui::Selectable((identity.name + "##" + entityGUID.toString()).c_str(), selected))
		{
			if (m_SelectedEntity == entityGUID)
				SelectEntity(GUID::INVALID_GUID);
			else
				SelectEntity(entityGUID);
		}
		if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight))
		{
			SelectEntity(entityGUID);
			if (ImGui::MenuItem("Remove"))
			{
				m_World->DestroyEntity(e);
			}
			ImGui::EndPopup();
		}
	}
	ImGui::EndChild();

	SceneTreeHeight += GUI::Splitter();

	ImGui::BeginChild("SpawnEntity", ImVec2(0, 150), true);
	ImGui::PopStyleVar();

	static std::string entityName;
	if ((ImGui::InputText("Entity name", &entityName, ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::Button("Spawn")) && !entityName.empty())
	{
		const auto entity = m_World->CreateEntity(entityName);
		SelectEntity(entity.GetGuid());
		entityName = "";
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::EndChild();
	ImGui::BeginChild("SelectedEntity", ImVec2(0, 0), true);
	ImGui::PopStyleVar();

	m_ComponentEditor.SetEntity(m_World->FindByGuid(m_SelectedEntity));
	m_ComponentEditor.Draw();
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::EndChild();

	ImGui::PopStyleVar();
	ImGui::End();
	return false;
}

//=================================================================================
void C_EntitiesWindow::SelectEntity(GUID id) const
{
	if (m_SelectedEntity != id)
	{
		m_SelectedEntity = id;
		Core::C_EntityEvent event(m_SelectedEntity, Core::C_EntityEvent::EntityEvent::Selected);
		Core::C_Application::Get().OnEvent(event);
	}
}

//=================================================================================
void C_EntitiesWindow::SetWorld(Entity::C_World& world)
{
	m_World = &world;
	// entity removed anyway
	SelectEntity(GUID::INVALID_GUID);
}

} // namespace GLEngine::Editor
