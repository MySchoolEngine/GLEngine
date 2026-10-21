#include <EntityStdafx.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/Systems/TransformSystem.h>
#include <Entity/World.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

namespace GLEngine::Entity::TransformSystem {

namespace {

glm::mat4 ComputeLocal(const S_TransformComponent& t)
{
	return glm::translate(glm::mat4(1.f), t.translation) * glm::mat4_cast(t.rotation) * glm::scale(glm::mat4(1.f), t.scale);
}

// Recomputes from this node up to its root every call. Not pool-sorted or
// cached across calls - correct but O(depth) per dirty node. Sorting the
// transform pool by hierarchy depth is an explicit later optimization; the
// BM_WorldIterate benchmark is the signal for whether it's needed.
glm::mat4 ComputeWorld(entt::registry& registry, entt::entity e)
{
	const auto& transform = registry.get<S_TransformComponent>(e);
	const auto	local	  = ComputeLocal(transform);
	const auto& rel		  = registry.get<S_RelationshipComponent>(e);
	if (rel.parent == entt::null)
		return local;
	return ComputeWorld(registry, rel.parent) * local;
}

void PropagateFrom(entt::registry& registry, entt::entity node)
{
	registry.get<S_WorldTransformComponent>(node).world = ComputeWorld(registry, node);
	const auto& rel										= registry.get<S_RelationshipComponent>(node);
	for (auto child = rel.firstChild; child != entt::null; child = registry.get<S_RelationshipComponent>(child).next)
	{
		registry.emplace_or_replace<S_DirtyTransformTag>(child);
		PropagateFrom(registry, child);
	}
}

} // namespace

void Update(C_World& world)
{
	auto&					  registry	= world.Registry();
	const auto				  dirtyView = registry.view<S_DirtyTransformTag>();
	std::vector<entt::entity> dirty(dirtyView.begin(), dirtyView.end());
	for (const auto e : dirty)
	{
		PropagateFrom(registry, e);
	}
	registry.clear<S_DirtyTransformTag>();
}

} // namespace GLEngine::Entity::TransformSystem
