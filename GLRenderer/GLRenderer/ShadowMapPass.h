#pragma once

#include <GLRenderer/FBO/Framebuffer.h>

namespace GLEngine::Entity {
class C_EntityManager;
}

namespace GLEngine::Renderer {
class I_CameraComponent;
class I_Light;
} // namespace GLEngine::Renderer

namespace GLEngine::GLRenderer {

namespace Buffers::UBO {
class C_FrameConstantsBuffer;
}

class C_ShadowMapTechnique {
public:
	C_ShadowMapTechnique(std::shared_ptr<Entity::C_EntityManager> world, std::shared_ptr<Renderer::I_Light>& light);
	void Render();

private:
	std::shared_ptr<Entity::C_EntityManager>			  m_WorldToRender;
	std::shared_ptr<Renderer::I_Light>					  m_Light;
	std::shared_ptr<Buffers::UBO::C_FrameConstantsBuffer> m_FrameConstUBO;
	C_Framebuffer										  m_ShadowPassFBO;
	GLuint												  m_ColorRenderbuffer;
};

} // namespace GLEngine::GLRenderer