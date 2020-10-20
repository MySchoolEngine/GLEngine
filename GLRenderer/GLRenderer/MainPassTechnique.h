#pragma once

namespace GLEngine::Entity
{
class C_EntityManager;
}

namespace GLEngine::Renderer
{
class I_CameraComponent;
}

namespace GLEngine::GLRenderer {

class C_LightsBuffer;

namespace Buffers::UBO {
class C_FrameConstantsBuffer;
}

class C_MainPassTechnique {
public:
	explicit C_MainPassTechnique(std::shared_ptr<Entity::C_EntityManager> world);
	void Render(std::shared_ptr<Renderer::I_CameraComponent> camera, unsigned int widht, unsigned int height);

private:
	std::shared_ptr<Entity::C_EntityManager>				m_WorldToRender;
	std::shared_ptr<Buffers::UBO::C_FrameConstantsBuffer>	m_FrameConstUBO;
	std::shared_ptr<C_LightsBuffer>							m_LightsUBO;
};

}