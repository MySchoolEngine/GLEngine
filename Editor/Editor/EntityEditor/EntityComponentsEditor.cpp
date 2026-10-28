#include <EditorStdafx.h>

#include <Editor/EntityEditor/EntityComponentsEditor.h>

#include <GUI/Input/DrawTransform.h>
#include <GUI/ReflectionGUI.h>

#include <Entity/ComponentRegistry.h>
#include <Entity/Components/CoreComponents.h>
#include <Entity/World.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace GLEngine::Editor {

//=================================================================================
void C_EntityCompoenentEditor::SetEntity(Entity::C_Entity entity)
{
	m_Entity = entity;
}

//=================================================================================
bool C_EntityCompoenentEditor::Draw() const
{
	if (!m_Entity.IsValid())
		return false;

	ImGui::Text("%s", m_Entity.GetName().c_str());

	auto& registry = m_Entity.World()->Registry();
	int	  i		   = 0;
	for (const auto& entry : Entity::C_ComponentRegistry::Instance().Entries())
	{
		if (!entry.drawGUI || !entry.has(registry, m_Entity.Handle()))
			continue;

		ImGui::PushID(i);
		if (entry.userAddable)
			ImGui::SetNextItemAllowOverlap();
		const bool open = ImGui::CollapsingHeader(entry.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
		if (entry.userAddable)
		{
			ImGui::SameLine();
			if (ImGui::SmallButton("Remove"))
			{
				entry.remove(registry, m_Entity.Handle());
				ImGui::PopID();
				++i;
				continue;
			}
		}
		if (open)
		{
			if (entry.type == rttr::type::get<Entity::S_TransformComponent>())
			{
				auto& transform = registry.get<Entity::S_TransformComponent>(m_Entity.Handle());
				if (GUI::Input::DrawTransform(transform.translation, transform.rotation, transform.scale))
					registry.patch<Entity::S_TransformComponent>(m_Entity.Handle());
			}
			else
			{
				auto		   variant = entry.get(registry, m_Entity.Handle());
				rttr::instance inst(variant);
				if (!GUI::DrawAllPropertyGUI(inst).empty())
					entry.patch(registry, m_Entity.Handle());
			}
		}
		ImGui::PopID();
		++i;
	}

	ImGui::BeginChild("AddComponent", ImVec2(0, 100), true);
	const auto&			  entries		 = Entity::C_ComponentRegistry::Instance().Entries();
	constexpr static auto s_InvalidIndex = std::numeric_limits<std::size_t>::max();
	static auto			  selectedIndex	 = s_InvalidIndex;
	if (ImGui::BeginCombo("Component class", selectedIndex != s_InvalidIndex && selectedIndex < entries.size() ? entries[selectedIndex].name.c_str() : ""))
	{
		for (std::size_t entryIndex = 0; entryIndex < entries.size(); ++entryIndex)
		{
			if (!entries[entryIndex].userAddable)
				continue;
			if (ImGui::Selectable(entries[entryIndex].name.c_str(), entryIndex == selectedIndex))
				selectedIndex = entryIndex;
		}
		ImGui::EndCombo();
	}
	if (selectedIndex != s_InvalidIndex && entries[selectedIndex].has(registry, m_Entity.Handle()) == false)
	{
		if (ImGui::Button("Add component") && selectedIndex < entries.size())
		{
			entries[selectedIndex].emplaceDefault(registry, m_Entity.Handle());
		}
	}
	ImGui::EndChild();

	return false;
}

} // namespace GLEngine::Editor
