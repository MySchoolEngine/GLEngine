#include <EntityStdafx.h>

#include <Entity/Components/EntityDebugComponent.h>

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
		for (auto& component : *entity) {
			component.second->DebugDrawGUI();
		}
	}
}

//=================================================================================
std::shared_ptr<I_Component> C_GUIDebugBuilder::Build(const pugi::xml_node& node, std::shared_ptr<I_Entity> owner)
{
	return std::make_shared<C_EntityDebugComponent>(owner);
}

}