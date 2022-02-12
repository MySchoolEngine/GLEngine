#include <EntityStdafx.h>

#include <Entity/BasicEntity.h>
#include <Entity/Components/EntityDebugComponent.h>

#include <imgui.h>

namespace GLEngine::Entity {

//=================================================================================
C_EntityDebugComponent::C_EntityDebugComponent(std::shared_ptr<I_Entity> entity)
	: C_DebugGUIComponent(entity)
	, m_Entity(std::static_pointer_cast<C_BasicEntity>(entity))
{
}

//=================================================================================
C_EntityDebugComponent::~C_EntityDebugComponent() = default;

//=================================================================================
void C_EntityDebugComponent::DrawContents()
{
	if (auto entity = m_Entity.lock())
	{
		int i = 0;
		for (auto& component : *entity)
		{
			ImGui::PushID(i);
			if (component.second->HasDebugDrawGUI())
				component.second->DebugDrawComponentGUI();
			ImGui::PopID();
			++i;
		}
	}
}

//=================================================================================
std::string_view C_EntityDebugComponent::GetDebugComponentName() const
{
	return "Debug entity component";
}

//=================================================================================
std::shared_ptr<I_Component> C_GUIDebugBuilder::Build(const pugi::xml_node& node, std::shared_ptr<I_Entity> owner)
{
	return std::make_shared<C_EntityDebugComponent>(owner);
}

} // namespace GLEngine::Entity