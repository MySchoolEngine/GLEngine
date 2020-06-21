#include <EntityStdafx.h>

#include <Entity/BasicEntity.h>

#include <Entity/IComponent.h>

#include <Renderer/IDebugGUIComponent.h>


namespace GLEngine {
namespace Entity {

//=================================================================================
C_BasicEntity::C_BasicEntity(std::string name)
	: I_Entity(std::move(name))
	, m_ModelMatrix(glm::mat4(1.f))
{
}

//=================================================================================
C_BasicEntity::~C_BasicEntity() = default;

//=================================================================================
void C_BasicEntity::Update()
{
	for (auto comp : *m_Components)
	{
		comp.second->Update();
	}
}

//=================================================================================
void C_BasicEntity::PostUpdate()
{
	for (auto& comp : *m_Components)
	{
		comp.second->PostUpdate();
	}
}

//=================================================================================
void C_BasicEntity::OnEvent(Core::I_Event& event)
{
	auto debugGUI = GetComponent(Entity::E_ComponentType::DebugGUI);
	if (debugGUI) {
		GLEngine::component_cast<Entity::E_ComponentType::DebugGUI>(debugGUI)->Toggle();
	}
}

//=================================================================================
void C_BasicEntity::SetModelMatrix(const glm::mat4& modelMatrix)
{
	m_ModelMatrix = modelMatrix;
}

//=================================================================================
const glm::mat4& C_BasicEntity::GetModelMatrix() const
{
	return m_ModelMatrix;
}

//=================================================================================
glm::vec3 C_BasicEntity::GetPosition() const
{
	return m_ModelMatrix[3];
}

}}