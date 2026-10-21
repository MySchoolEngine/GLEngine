#include <RendererStdafx.h>

#include <Renderer/Components/Lights/LightComponents.h>
#include <Renderer/Components/Lights/LightFunctions.h>
#include <Renderer/Systems/LightSystem.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/World.h>

namespace GLEngine::Renderer::LightSystem {

void Update(Entity::C_World& world)
{
	auto& registry = world.Registry();

	for (auto [e, light, worldTransform] : registry.view<S_PointLight, Entity::S_WorldTransformComponent>().each())
	{
		registry.emplace_or_replace<Entity::S_LocalBounds>(e, GetAABB(light, worldTransform));
	}
	for (auto [e, light, worldTransform] : registry.view<S_AreaLight, Entity::S_WorldTransformComponent>().each())
	{
		registry.emplace_or_replace<Entity::S_LocalBounds>(e, GetAABB(light, worldTransform));
	}
	for (auto [e, light, worldTransform] : registry.view<S_SunLight, Entity::S_WorldTransformComponent>().each())
	{
		registry.emplace_or_replace<Entity::S_LocalBounds>(e, GetAABB(light, worldTransform));
	}
}

} // namespace GLEngine::Renderer::LightSystem
