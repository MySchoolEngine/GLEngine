#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/ICameraComponent.h>

#include <Entity/IEntity.h>
#include <Entity/World.h>

#include <vector>
#include <memory>

namespace GLEngine {
namespace GLRenderer {
class API_EXPORT C_MainPassTechnique {
public:
	C_MainPassTechnique(std::shared_ptr<Entity::C_World> world, std::shared_ptr<Renderer::I_CameraComponent> camera);
	void Render();

private:
	std::shared_ptr<Entity::C_World> m_WorldToRender;
};
}
}