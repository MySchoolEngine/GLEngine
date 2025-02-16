#pragma once

namespace GLEngine::Entity {
class C_EntityManager;
}

namespace GLEngine::Renderer {
class C_SunLight;
class I_CameraComponent;
} // namespace GLEngine::Renderer

namespace GLEngine::GLRenderer {
namespace Buffers::UBO {
class C_FrameConstantsBuffer;
}

class C_Framebuffer;

class C_SunShadowMapTechnique final {
public:
	explicit C_SunShadowMapTechnique(const std::shared_ptr<Renderer::C_SunLight>& light);
	~C_SunShadowMapTechnique(); // = default

	void								 Render(const Entity::C_EntityManager& world, Renderer::I_CameraComponent* camera);
	Renderer::Handle<Renderer::Texture>	 GetZBuffer() const;
	Renderer::Handle<Renderer::Texture>	 GetAlbedoTexture() const;

	glm::mat4 GetLastViewProjection() const;

private:
	std::weak_ptr<Renderer::C_SunLight>					  m_Sun;
	std::shared_ptr<Buffers::UBO::C_FrameConstantsBuffer> m_FrameConstUBO;
	std::unique_ptr<C_Framebuffer>						  m_Framebuffer;
	glm::mat4											  m_LastViewProject;

	const static std::uint32_t s_ShadowMapSize;
	const static float		   s_ZOffset;
};
} // namespace GLEngine::GLRenderer
