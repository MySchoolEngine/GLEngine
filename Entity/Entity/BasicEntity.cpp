#include <EntityStdafx.h>

#include <Entity/BasicEntity.h>

#include <Entity/IComponent.h>

#include <Renderer/IDebugGUIComponent.h>


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
void C_BasicEntity::Update()
{

}

//=================================================================================
void C_BasicEntity::PostUpdate()
{
	for (auto comp : *m_Components)
	{
		comp.second->PostUpdate();
	}
}

//=================================================================================
void C_BasicEntity::AddComponent(T_ComponentPtr component)
{
	m_Components->insert({ component->GetType(), component });
}

//=================================================================================
GLEngine::Entity::I_Entity::EntityID C_BasicEntity::GetID() const
{
	return m_ID;
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
C_BasicEntity::T_ComponentIter C_BasicEntity::begin()
{
	return m_Components->begin();
}

//=================================================================================
C_BasicEntity::T_ComponentIter C_BasicEntity::end()
{
	return m_Components->end();
}

}}