#pragma once

#include <Renderer/Resources/RenderResourceHandle.h>

namespace GLEngine::Entity {
class C_EntityManager;
}

namespace GLEngine::Renderer {
class ResourceManager;
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
	explicit C_MainPassTechnique(Renderer::ResourceManager& resourceManager);
	void Render(const Entity::C_EntityManager& world, std::shared_ptr<Renderer::I_CameraComponent> camera, unsigned int width, unsigned int height);
	void SetSunShadowMap(Renderer::Handle<Renderer::Texture> sunShadowMapHandle);
	void SetSunViewProjection(glm::mat4 viewProjection);

private:
	std::shared_ptr<Buffers::UBO::C_FrameConstantsBuffer> m_FrameConstUBO;
	std::shared_ptr<C_LightsBuffer>						  m_LightsUBO;
	Renderer::Handle<Renderer::Texture>					  m_SunShadowMap;
	glm::mat4											  m_SunViewProjection;
	std::shared_ptr<Material::C_MaterialsBuffer>		  m_MaterialsUBO;
};

} // namespace GLEngine::GLRenderer