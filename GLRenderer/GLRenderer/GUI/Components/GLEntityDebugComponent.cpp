#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/Components/GLEntityDebugComponent.h>
#define GLM_GTC_matrix_transform
#include <glm/gtc/matrix_transform.hpp>

namespace GLEngine {
namespace GLRenderer {
namespace GUI {

//=================================================================================
C_GLEntityDebugComponent::C_GLEntityDebugComponent(std::shared_ptr<Entity::I_Entity> entity)
	: C_GLDebugGUIComponent(entity)
	, m_Entity(std::static_pointer_cast<Entity::C_BasicEntity>(entity))
	, m_Translation("Transformation", entity->GetPosition())
{

}

//=================================================================================
void C_GLEntityDebugComponent::DrawContents()
{
	if (auto entity = m_Entity.lock())
	{
		m_Translation.Draw();
		for (auto& component : *entity) {
			component.second->DebugDrawGUI();
		}
		entity->SetModelMatrix(glm::translate(glm::mat4(1.f), m_Translation.GetValue()));
	}
}

//=================================================================================
std::shared_ptr<GLEngine::Entity::I_Component> C_GUIDebugBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	return std::make_shared<C_GLEntityDebugComponent>(owner);
}

}}}