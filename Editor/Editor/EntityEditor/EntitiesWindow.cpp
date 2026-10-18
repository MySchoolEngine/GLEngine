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

namespace {
constexpr const char* kReparentDragDropId = "ENTITIES_WINDOW_REPARENT";
}

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
void C_EntitiesWindow::DrawEntityNode(entt::entity e) const
{
	auto&		registry = m_World->Registry();
	const auto& identity = registry.get<Entity::S_IdentityComponent>(e);
	const auto& rel		 = registry.get<Entity::S_RelationshipComponent>(e);

	const bool selected	   = identity.id == m_SelectedEntity;
	const bool hasChildren = rel.firstChild != entt::null;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (selected)
		flags |= ImGuiTreeNodeFlags_Selected;
	if (!hasChildren)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	ImGui::PushID(static_cast<int>(entt::to_integral(e)));
	const bool open = ImGui::TreeNodeEx(identity.name.c_str(), flags);

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		SelectEntity(identity.id);

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload(kReparentDragDropId, &e, sizeof(entt::entity));
		ImGui::Text("Reparent '%s'", identity.name.c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const auto* payload = ImGui::AcceptDragDropPayload(kReparentDragDropId))
		{
			m_PendingReparentChild	= *static_cast<const entt::entity*>(payload->Data);
			m_PendingReparentTarget = e;
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight))
	{
		SelectEntity(identity.id);
		if (ImGui::MenuItem("Create child"))
			m_PendingCreateChildOf = e;
		if (ImGui::MenuItem("Remove"))
			m_PendingRemove = e;
		ImGui::EndPopup();
	}

	if (hasChildren && open)
	{
		for (auto child = rel.firstChild; child != entt::null; child = registry.get<Entity::S_RelationshipComponent>(child).next)
			DrawEntityNode(child);
		ImGui::TreePop();
	}
	ImGui::PopID();
}

//=================================================================================
bool C_EntitiesWindow::Draw(GUI::C_GUIManager& guiMgr) const
{
	ImGui::Begin(m_Name.c_str(), &m_IsVisible);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	static float SceneTreeHeight = 300.0f;
	ImGui::BeginChild("SceneTree", ImVec2(0, std::max(50.f, SceneTreeHeight)), true);

	auto& registry = m_World->Registry();

	m_PendingReparentChild	= entt::null;
	m_PendingReparentTarget = entt::null;
	m_PendingRemove			= entt::null;
	m_PendingCreateChildOf	= entt::null;

	// Snapshot roots before recursing: SetParent/DestroyEntity are only ever applied below, after
	// the walk completes, but drag-drop target entities are discovered mid-walk, so the walk
	// itself must never mutate the firstChild/next lists it's reading.
	std::vector<entt::entity> roots;
	for (auto [e, rel] : registry.view<Entity::S_RelationshipComponent>().each())
	{
		if (rel.parent == entt::null)
			roots.push_back(e);
	}
	for (const auto root : roots)
		DrawEntityNode(root);

	ImGui::EndChild();

	if (m_PendingRemove != entt::null)
	{
		// DestroyEntity defers to the next OnUpdate(), so the entity is still "valid" this frame -
		// compare the GUID directly rather than re-checking validity.
		if (registry.get<Entity::S_IdentityComponent>(m_PendingRemove).id == m_SelectedEntity)
			SelectEntity(GUID::INVALID_GUID);
		m_World->DestroyEntity(m_PendingRemove);
	}
	if (m_PendingCreateChildOf != entt::null)
	{
		auto child	= m_World->CreateEntity("New Entity");
		std::ignore = m_World->SetParent(child.Handle(), m_PendingCreateChildOf);
		SelectEntity(child.GetGuid());
	}
	if (m_PendingReparentChild != entt::null && m_PendingReparentTarget != entt::null && m_PendingReparentChild != m_PendingReparentTarget)
	{
		if (!m_World->SetParent(m_PendingReparentChild, m_PendingReparentTarget))
		{
			m_StatusMessage			= "Can't reparent: would create a cycle";
			m_StatusMessageTimeLeft = 2.f;
		}
	}

	if (m_StatusMessageTimeLeft > 0.f)
	{
		m_StatusMessageTimeLeft -= ImGui::GetIO().DeltaTime;
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 96, 96, 255));
		ImGui::TextUnformatted(m_StatusMessage.c_str());
		ImGui::PopStyleColor();
	}

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
