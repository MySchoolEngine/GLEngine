#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/Components/GLEntityDebugComponent.h>

#include <Entity/BasicEntity.h>

namespace GLEngine {
namespace GLRenderer {
namespace GUI {

//=================================================================================
C_GLEntityDebugComponent::C_GLEntityDebugComponent(std::shared_ptr<Entity::I_Entity> entity)
	: C_GLDebugGUIComponent(entity)
	, m_Entity(std::static_pointer_cast<Entity::C_BasicEntity>(entity))
{

}

//=================================================================================
void C_GLEntityDebugComponent::DrawContents()
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
	return std::make_shared<C_GLEntityDebugComponent>(owner);
}

}}}