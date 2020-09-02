#include <GUIStdafx.h>

#include <GUI/Components/GLEntityDebugComponent.h>


namespace GLEngine::GUI {

//=================================================================================
C_EntityDebugComponent::C_EntityDebugComponent(std::shared_ptr<Entity::I_Entity> entity)
	: C_DebugGUIComponent(entity)
	, m_Entity(std::static_pointer_cast<Entity::C_BasicEntity>(entity))
{

}

//=================================================================================
void C_EntityDebugComponent::DrawContents()
{
	if (auto entity = m_Entity.lock())
	{
		for (auto& component : *entity) {
			component.second->DebugDrawGUI();
		}
	}
}

//=================================================================================
std::shared_ptr<GLEngine::Entity::I_Component> C_GUIDebugBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	return std::make_shared<C_EntityDebugComponent>(owner);
}

}