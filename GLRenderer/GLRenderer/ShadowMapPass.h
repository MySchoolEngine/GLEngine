#pragma once

#include <GLRenderer/FBO/Framebuffer.h>

namespace GLEngine::Entity {
class C_World;
}

namespace GLEngine::Renderer {
class I_Camera;
class I_Light;
} // namespace GLEngine::Renderer

namespace GLEngine::GLRenderer {

namespace Buffers::UBO {
class C_FrameConstantsBuffer;
}

// completly broken now
class C_ShadowMapTechnique {
public:
	C_ShadowMapTechnique(Entity::C_World& world, const std::shared_ptr<Renderer::I_Light>& light, Renderer::I_Device& device);
	~C_ShadowMapTechnique();
	void Render();

private:
	Entity::C_World&									  m_WorldToRender;
	std::shared_ptr<Renderer::I_Light>					  m_Light;
	std::shared_ptr<Buffers::UBO::C_FrameConstantsBuffer> m_FrameConstUBO;
	C_Framebuffer*										  m_ShadowPassFBO;
	GLuint												  m_ColorRenderbuffer;
};

} // namespace GLEngine::GLRenderer