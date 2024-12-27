#pragma once

#include <Renderer/Resources/RenderResourceHandle.h>

#include <Renderer/Lights/AreaLight.h>

namespace GLEngine::GLRenderer {
namespace Textures {
class C_Texture;
}

class C_GLAreaLight : public Renderer::C_AreaLight {
public:
	explicit C_GLAreaLight(std::shared_ptr<Entity::I_Entity> owner);
	Renderer::Handle<Renderer::Texture> GetShadowMap() const;

private:
	Renderer::Handle<Renderer::Texture> m_ShadowMap;
};
} // namespace GLEngine::GLRenderer
