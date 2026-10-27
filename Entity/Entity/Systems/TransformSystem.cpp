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
// Walks up to the root with an explicit loop rather than recursing once per level, so a very
// deep hierarchy (an imported model's node chain, or a generated test) can't exhaust the stack.
glm::mat4 ComputeWorld(entt::registry& registry, entt::entity e)
{
	std::vector<glm::mat4> localsLeafToRoot;
	for (entt::entity node = e; node != entt::null; node = registry.get<S_RelationshipComponent>(node).parent)
		localsLeafToRoot.push_back(ComputeLocal(registry.get<S_TransformComponent>(node)));

	glm::mat4 world(1.f);
	for (auto it = localsLeafToRoot.rbegin(); it != localsLeafToRoot.rend(); ++it)
		world = world * (*it);
	return world;
}

// Explicit stack instead of recursing into children, for the same reason as ComputeWorld above -
// a deep/wide subtree must not be bounded by the call stack. Sibling processing order (LIFO, so
// reversed from the old left-to-right recursion) doesn't affect the result: each entity's world
// matrix depends only on its own ancestor chain, never on sibling order.
void PropagateFrom(entt::registry& registry, entt::entity root)
{
	std::vector<entt::entity> pending{root};
	while (!pending.empty())
	{
		const auto node = pending.back();
		pending.pop_back();

		registry.get<S_WorldTransformComponent>(node).world = ComputeWorld(registry, node);
		const auto& rel										= registry.get<S_RelationshipComponent>(node);
		for (auto child = rel.firstChild; child != entt::null; child = registry.get<S_RelationshipComponent>(child).next)
		{
			registry.emplace_or_replace<S_DirtyTransformTag>(child);
			pending.push_back(child);
		}
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
