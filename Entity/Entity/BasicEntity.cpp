#include <EntityStdafx.h>

#include <Entity/BasicEntity.h>

#include <Entity/IComponent.h>


namespace GLEngine {
namespace Entity {

//=================================================================================
C_BasicEntity::C_BasicEntity(std::string name)
	: m_Name(std::move(name))
	, m_Components(new std::remove_pointer<decltype(m_Components)>::type)
	, m_ID(NextGUID())
{
	CORE_LOG(E_Level::Info, E_Context::Entity, "Entity '{}' created.", m_Name);
}

//=================================================================================
C_BasicEntity::~C_BasicEntity()
{
	delete m_Components;
}

//=================================================================================
T_ComponentPtr C_BasicEntity::GetComponent(E_ComponentType type) const
{
	auto iter = m_Components->find(type);
	if (iter != m_Components->end()) {
		return iter->second;
	}

	return nullptr;
}

//=================================================================================
std::string C_BasicEntity::GetName() const
{
	return m_Name;
}

//=================================================================================
void C_BasicEntity::AddComponent(std::shared_ptr<I_Component> component)
{
	m_Components->insert({ component->GetType(), component });
}

//=================================================================================
GLEngine::Entity::I_Entity::EntityID C_BasicEntity::GetID() const
{
	return m_ID;
}

}}