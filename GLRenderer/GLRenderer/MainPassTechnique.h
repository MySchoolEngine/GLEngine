#pragma once

#include <Renderer/ICameraComponent.h>

#include <Entity/IEntity.h>
#include <Entity/EntityManager.h>

namespace GLEngine::GLRenderer {

namespace Buffers::UBO {
class C_FrameConstantsBuffer;
}

class C_MainPassTechnique {
public:
	C_MainPassTechnique(std::shared_ptr<Entity::C_EntityManager> world);
	void Render(std::shared_ptr<Renderer::I_CameraComponent> camera, unsigned int widht, unsigned int height);

private:
	std::shared_ptr<Entity::C_EntityManager>								m_WorldToRender;
	std::shared_ptr<Buffers::UBO::C_FrameConstantsBuffer>		m_FrameConstUBO;
};

}