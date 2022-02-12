#pragma once

#include <Renderer/Lights/AreaLight.h>

namespace GLEngine::GLRenderer {
namespace Textures {
class C_Texture;
}

class C_GLAreaLight : public Renderer::C_AreaLight {
public:
	explicit C_GLAreaLight(std::shared_ptr<Entity::I_Entity> owner);
	const std::shared_ptr<Textures::C_Texture>& GetShadowMap() const;

private:
	std::shared_ptr<Textures::C_Texture> m_ShadowMap;
};
} // namespace GLEngine::GLRenderer
