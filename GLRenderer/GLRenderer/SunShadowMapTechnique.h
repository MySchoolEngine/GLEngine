#pragma once

#include <Renderer/Renderer3D.h>

#include <entt/entt.hpp>

namespace GLEngine::Entity {
class C_World;
}

namespace GLEngine::Renderer {
class I_Camera;
}

namespace GLEngine::GLRenderer {
namespace Buffers::UBO {
class C_FrameConstantsBuffer;
}

class C_Framebuffer;

class C_SunShadowMapTechnique final {
public:
	explicit C_SunShadowMapTechnique(entt::entity sun);
	~C_SunShadowMapTechnique(); // = default

	void								Render(Entity::C_World& world, Renderer::I_Camera* camera, Renderer::I_RenderInterface3D& renderInterface3D);
	Renderer::Handle<Renderer::Texture> GetZBuffer() const;
	Renderer::Handle<Renderer::Texture> GetAlbedoTexture() const;

	glm::mat4 GetLastViewProjection() const;

private:
	entt::entity										  m_Sun;
	std::shared_ptr<Buffers::UBO::C_FrameConstantsBuffer> m_FrameConstUBO;
	std::unique_ptr<C_Framebuffer>						  m_Framebuffer;
	glm::mat4											  m_LastViewProject;
	Renderer::Renderer3D								  m_3DRenderer;

	const static std::uint32_t s_ShadowMapSize;
	const static float		   s_ZOffset;
};
} // namespace GLEngine::GLRenderer
