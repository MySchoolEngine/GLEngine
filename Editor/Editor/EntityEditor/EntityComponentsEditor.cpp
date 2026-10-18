#include <EditorStdafx.h>

#include <Editor/EntityEditor/EntityComponentsEditor.h>

#include <GUI/ReflectionGUI.h>

#include <Entity/ComponentRegistry.h>
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
			auto		   variant = entry.get(registry, m_Entity.Handle());
			rttr::instance inst(variant);
			GUI::DrawAllPropertyGUI(inst);
		}
		ImGui::PopID();
		++i;
	}

	ImGui::BeginChild("AddComponent", ImVec2(0, 100), true);
	const auto& entries		  = Entity::C_ComponentRegistry::Instance().Entries();
	static int	selectedIndex = -1;
	if (ImGui::BeginCombo("Component class", selectedIndex >= 0 && selectedIndex < static_cast<int>(entries.size()) ? entries[selectedIndex].name.c_str() : ""))
	{
		for (int entryIndex = 0; entryIndex < static_cast<int>(entries.size()); ++entryIndex)
		{
			if (!entries[entryIndex].userAddable)
				continue;
			if (ImGui::Selectable(entries[entryIndex].name.c_str(), entryIndex == selectedIndex))
				selectedIndex = entryIndex;
		}
		ImGui::EndCombo();
	}
	if (ImGui::Button("Add component") && selectedIndex >= 0 && selectedIndex < static_cast<int>(entries.size()))
	{
		entries[selectedIndex].emplaceDefault(registry, m_Entity.Handle());
	}
	ImGui::EndChild();

	return false;
}

} // namespace GLEngine::Editor
