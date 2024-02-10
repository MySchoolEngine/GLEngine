#include <EditorStdafx.h>

#include <Editor/EntityEditor/EntityComponentsEditor.h>

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace GLEngine::Editor {

//=================================================================================
C_EntityCompoenentEditor::C_EntityCompoenentEditor()
	: m_Entity()
	, m_ComponentTypeSelector("Component class", rttr::type::get<Entity::I_Component>().get_name().data())
{
}

//=================================================================================
void C_EntityCompoenentEditor::SetEntity(std::shared_ptr<Entity::I_Entity> entity)
{
	m_Entity = entity;
}

//=================================================================================
bool C_EntityCompoenentEditor::Draw() const
{
	if (auto entity = m_Entity.lock())
	{
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

		ImGui::BeginChild("AddComponent", ImVec2(0, 100), true);
		m_ComponentTypeSelector.Draw();
		if (ImGui::Button("Add component"))
		{
			const auto type = rttr::type::get_by_name(m_ComponentTypeSelector.GetSelectedTypeName());
			GLE_ASSERT(type.is_valid(), "Type {} doesn't exists.", m_ComponentTypeSelector.GetSelectedTypeName());
			auto component = type.create();
			GLE_ASSERT(component, "Cannot instantiate component '{}'.", m_ComponentTypeSelector.GetSelectedTypeName());
			auto componentPtr = component.convert<std::shared_ptr<Entity::I_Component>>();
			componentPtr->SetParent(entity);
			entity->AddComponent(componentPtr);
		}
		ImGui::EndChild();
	}
	return false;
}

} // namespace GLEngine::Editor