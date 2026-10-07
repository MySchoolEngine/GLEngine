#include <EntityStdafx.h>

#include <Entity/DllBoundarySpike.h>

namespace GLEngine::Entity::Spike {

entt::registry& GetRegistry()
{
	static entt::registry registry;
	return registry;
}

} // namespace GLEngine::Entity::Spike
