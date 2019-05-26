#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/Components/GLEntityDebugComponent.h>


namespace GLEngine {
namespace GLRenderer {
namespace GUI {

//=================================================================================
C_GLEntityDebugComponent::C_GLEntityDebugComponent(std::shared_ptr<Entity::C_BasicEntity> entity)
	: C_GLDebugGUIComponent(entity->GetName())
	, m_Entity(entity)
{

}

//=================================================================================
void C_GLEntityDebugComponent::DrawContents()
{
	for (auto& component : *m_Entity) {
		component.second->DebugDrawGUI();
	}
}

}}}