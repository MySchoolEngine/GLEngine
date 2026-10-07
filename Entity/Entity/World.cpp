#include <EntityStdafx.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/World.h>

namespace GLEngine::Entity {

//=================================================================================
C_Entity C_World::CreateEntity(std::string name)
{
	const auto handle = m_Registry.create();
	const GUID guid	  = NextGUID();
	m_Registry.emplace<S_IdentityComponent>(handle, guid, std::move(name));
	m_GuidLookup.emplace(guid, handle);
	return C_Entity(*this, handle);
}

//=================================================================================
void C_World::DestroyEntity(entt::entity entity)
{
	m_PendingDestroy.push_back(entity);
}

//=================================================================================
void C_World::ClearLevel()
{
	m_Registry.clear();
	m_GuidLookup.clear();
	m_PendingDestroy.clear();
	m_Filename.clear();
}

//=================================================================================
C_Entity C_World::FindByGuid(const GUID& id)
{
	const auto it = m_GuidLookup.find(id);
	if (it == m_GuidLookup.end())
		return C_Entity();
	return C_Entity(*this, it->second);
}

//=================================================================================
C_Entity C_World::FindByName(const std::string& name)
{
	auto view = m_Registry.view<S_IdentityComponent>();
	for (auto [entity, identity] : view.each())
	{
		if (identity.name == name)
			return C_Entity(*this, entity);
	}
	return C_Entity();
}

//=================================================================================
void C_World::OnUpdate()
{
	for (const auto entity : m_PendingDestroy)
	{
		if (!m_Registry.valid(entity))
			continue;
		const auto& identity = m_Registry.get<S_IdentityComponent>(entity);
		m_GuidLookup.erase(identity.id);
		m_Registry.destroy(entity);
	}
	m_PendingDestroy.clear();
}

//=================================================================================
void C_World::SetFilename(const std::filesystem::path& filename)
{
	m_Filename = filename;
}

//=================================================================================
std::filesystem::path C_World::GetFilename() const
{
	return m_Filename;
}

} // namespace GLEngine::Entity
