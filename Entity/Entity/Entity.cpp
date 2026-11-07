#include <EntityStdafx.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/World.h>

namespace GLEngine::Entity {

namespace {
const std::string g_InvalidName;
}

//=================================================================================
C_Entity::C_Entity(C_World& world, entt::entity handle)
	: m_World(&world)
	, m_Handle(handle)
{
}

//=================================================================================
bool C_Entity::IsValid() const
{
	return m_World != nullptr && m_Handle != entt::null && m_World->Registry().valid(m_Handle);
}

//=================================================================================
GUID C_Entity::GetGuid() const
{
	if (!IsValid())
		return GUID::INVALID_GUID;
	return m_World->Registry().get<S_IdentityComponent>(m_Handle).id;
}

//=================================================================================
const std::string& C_Entity::GetName() const
{
	if (!IsValid())
		return g_InvalidName;
	return m_World->Registry().get<S_IdentityComponent>(m_Handle).name;
}

} // namespace GLEngine::Entity
