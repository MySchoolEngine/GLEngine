#include <EntityStdafx.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/World.h>

namespace GLEngine::Entity {

//=================================================================================
C_Entity C_World::CreateEntity(std::string name)
{
	return CreateEntityWithGuid(NextGUID(), std::move(name));
}

//=================================================================================
C_Entity C_World::CreateEntityWithGuid(const GUID& guid, std::string name)
{
	const auto handle = m_Registry.create();
	m_Registry.emplace<S_IdentityComponent>(handle, guid, std::move(name));
	m_Registry.emplace<S_TransformComponent>(handle);
	m_Registry.emplace<S_WorldTransformComponent>(handle);
	m_Registry.emplace<S_RelationshipComponent>(handle);
	m_Registry.emplace<S_DirtyTransformTag>(handle);
	m_GuidLookup.emplace(guid, handle);
	return C_Entity(*this, handle);
}

//=================================================================================
bool C_World::SetParent(entt::entity child, entt::entity newParent)
{
	if (child == newParent)
		return false;

	// Reject a cycle: newParent must not be child itself or a descendant of child.
	for (auto walk = newParent; walk != entt::null; walk = m_Registry.get<S_RelationshipComponent>(walk).parent)
	{
		if (walk == child)
			return false;
	}

	Detach(child);

	auto& childRel = m_Registry.get<S_RelationshipComponent>(child);
	if (newParent != entt::null)
	{
		auto& parentRel = m_Registry.get<S_RelationshipComponent>(newParent);
		childRel.next	= parentRel.firstChild;
		if (parentRel.firstChild != entt::null)
			m_Registry.get<S_RelationshipComponent>(parentRel.firstChild).prev = child;
		parentRel.firstChild = child;
		++parentRel.childCount;
	}
	childRel.parent = newParent;
	m_Registry.emplace_or_replace<S_DirtyTransformTag>(child);
	return true;
}

//=================================================================================
void C_World::Detach(entt::entity child)
{
	auto& childRel = m_Registry.get<S_RelationshipComponent>(child);
	if (childRel.parent == entt::null)
		return;

	auto& parentRel = m_Registry.get<S_RelationshipComponent>(childRel.parent);
	if (childRel.prev != entt::null)
		m_Registry.get<S_RelationshipComponent>(childRel.prev).next = childRel.next;
	else
		parentRel.firstChild = childRel.next;

	if (childRel.next != entt::null)
		m_Registry.get<S_RelationshipComponent>(childRel.next).prev = childRel.prev;

	--parentRel.childCount;
	childRel.parent = entt::null;
	childRel.prev	= entt::null;
	childRel.next	= entt::null;
	m_Registry.emplace_or_replace<S_DirtyTransformTag>(child);
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
