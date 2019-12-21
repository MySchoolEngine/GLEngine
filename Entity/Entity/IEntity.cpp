#include <EntityStdafx.h>

#include <Entity/IEntity.h>


namespace GLEngine {
namespace Entity {

//=================================================================================
I_Entity::I_Entity(std::string name)
	: m_Name(std::move(name))
	, m_Components(new std::remove_pointer<decltype(m_Components)>::type)
	, m_ID(NextGUID())
{
	CORE_LOG(E_Level::Info, E_Context::Entity, "Entity '{}' created.", m_Name);
}

//=================================================================================
I_Entity::~I_Entity()
{
	delete m_Components;
}

//=================================================================================
GLEngine::T_ComponentPtr I_Entity::GetComponent(E_ComponentType type) const
{
	auto iter = m_Components->find(type);
	if (iter != m_Components->end()) {
		return iter->second;
	}

	return nullptr;
}

//=================================================================================
void I_Entity::AddComponent(T_ComponentPtr component)
{
	m_Components->insert({ component->GetType(), component });
}


//=================================================================================
I_Entity::T_ComponentIter I_Entity::begin()
{
	return m_Components->begin();
}

//=================================================================================
I_Entity::T_ComponentIter I_Entity::end()
{
	return m_Components->end();
}

}}