#include <EntityStdafx.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/Systems/TransformSystem.h>
#include <Entity/World.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <unordered_set>
#include <vector>

namespace GLEngine::Entity::TransformSystem {

namespace {

glm::mat4 ComputeLocal(const S_TransformComponent& t)
{
	return glm::translate(glm::mat4(1.f), t.translation) * glm::mat4_cast(t.rotation) * glm::scale(glm::mat4(1.f), t.scale);
}

} // namespace

// Seeding only with "dirty roots" (a dirty node whose parent isn't also dirty this frame) matters
// for correctness, not just efficiency: registry.view<S_DirtyTransformTag>()'s iteration order
// does not respect hierarchy depth, so a child could be seeded and processed before its own
// parent's new world is ready. Any dirty node that isn't a root is still visited - in guaranteed
// parent-before-child order - when its nearest dirty ancestor is processed below, via the
// worklist. `queued` keeps a node from being seeded as a root AND later re-discovered as a
// descendant (both would otherwise still be visited exactly once, but tracking it is cheap and
// makes "exactly once" explicit rather than incidental).
void Update(C_World& world)
{
	auto& registry = world.Registry();

	std::vector<entt::entity> worklist;
	for (auto e : registry.view<S_DirtyTransformTag>())
	{
		const auto parent = registry.get<S_RelationshipComponent>(e).parent;
		if (parent == entt::null || !registry.all_of<S_DirtyTransformTag>(parent))
			worklist.push_back(e);
	}
	registry.clear<S_DirtyTransformTag>();

	std::unordered_set<entt::entity> queued(worklist.begin(), worklist.end());
	for (std::size_t i = 0; i < worklist.size(); ++i)
	{
		const auto	node	  = worklist[i];
		const auto& transform = registry.get<S_TransformComponent>(node);
		const auto	local	  = ComputeLocal(transform);
		const auto& rel		  = registry.get<S_RelationshipComponent>(node);

		registry.get<S_WorldTransformComponent>(node).world = rel.parent == entt::null ? local : registry.get<S_WorldTransformComponent>(rel.parent).world * local;

		for (auto child = rel.firstChild; child != entt::null; child = registry.get<S_RelationshipComponent>(child).next)
		{
			if (queued.insert(child).second)
				worklist.push_back(child);
		}
	}
}

} // namespace GLEngine::Entity::TransformSystem
