#pragma once

namespace GLEngine::Entity {
class C_EntityManager;
}

namespace GLEngine::Renderer {
class I_CameraComponent;
}

namespace GLEngine::GLRenderer {

class C_LightsBuffer;

namespace Buffers::UBO {
class C_FrameConstantsBuffer;
}

namespace Material {
class C_MaterialsBuffer;
}

class C_MainPassTechnique {
public:
	explicit C_MainPassTechnique();
	void Render(const Entity::C_EntityManager& world, std::shared_ptr<Renderer::I_CameraComponent> camera, unsigned int widht, unsigned int height);
	void SetSunShadowMap(std::uint64_t sunShadowMapHandle);
	void SetSunViewProjection(glm::mat4 viewProjection);

private:
	std::shared_ptr<Buffers::UBO::C_FrameConstantsBuffer> m_FrameConstUBO;
	std::shared_ptr<C_LightsBuffer>						  m_LightsUBO;
	std::uint64_t										  m_SunShadowMap;
	glm::mat4											  m_SunViewProjection;
	std::shared_ptr<Material::C_MaterialsBuffer>		  m_MatterialsUBO;
};

} // namespace GLEngine::GLRenderer