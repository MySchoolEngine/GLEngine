#include <RendererStdafx.h>

#include <Renderer/Components/CameraComponent.h>
#include <Renderer/ICamera.h>
#include <Renderer/Systems/CameraSystem.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/World.h>

namespace GLEngine::Renderer::CameraSystem {

void Update(Entity::C_World& world)
{
	auto& registry = world.Registry();

	for (auto [e, cameraComponent, worldTransform] : registry.view<S_CameraComponent, Entity::S_WorldTransformComponent>().each())
	{
		if (!cameraComponent.camera)
			continue;

		cameraComponent.camera->SetOwnerWorldTransform(worldTransform.world);
		cameraComponent.camera->Update();
	}
}

} // namespace GLEngine::Renderer::CameraSystem
