#pragma once

namespace GLEngine::Entity
{
class C_EntityManager;
}

namespace GLEngine::Renderer
{
class C_SunLight;
class I_CameraComponent;
}

namespace GLEngine::GLRenderer {
namespace Textures
{
class C_Texture;
}
namespace Buffers::UBO
{
class C_FrameConstantsBuffer;
}

class C_Framebuffer;

class C_SunShadowMapTechnique final
{
public:
	C_SunShadowMapTechnique(std::shared_ptr<Renderer::C_SunLight>& light);
	~C_SunShadowMapTechnique(); // = defualt

	void Render(const Entity::C_EntityManager& world, Renderer::I_CameraComponent* camera);
	std::shared_ptr<Textures::C_Texture> GetZBuffer() const;
	std::shared_ptr<Textures::C_Texture> GetAlbedoTexture() const;

	glm::mat4 GetLastViewProjection() const;
private:
	std::weak_ptr<Renderer::C_SunLight> m_Sun;
	std::shared_ptr<Buffers::UBO::C_FrameConstantsBuffer>	m_FrameConstUBO;
	std::unique_ptr<C_Framebuffer>		m_Framebuffer;
	glm::mat4							m_LastViewProject;

	const static std::uint32_t	s_ShadowMapSize;
	const static float			s_ZOffset;
};
}
