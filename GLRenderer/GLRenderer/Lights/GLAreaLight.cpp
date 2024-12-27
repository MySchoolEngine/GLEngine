#include <GLRendererStdafx.h>

#include <GLRenderer/Lights/GLAreaLight.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_GLAreaLight::C_GLAreaLight(std::shared_ptr<Entity::I_Entity> owner)
	: C_AreaLight(std::move(owner))
	//, m_ShadowMap(Textures::C_TextureManager::Instance().CreateEmptyTexture(owner->GetName() + "_ShadowMap"))
	, m_ShadowMap(Textures::C_TextureManager::Instance().GetIdentityTextureHandle())
{
	// m_ShadowMap->bind();
	// m_ShadowMap->SetDimensions({512, 512});
	// m_ShadowMap->SetInternalFormat(Renderer::E_TextureFormat::D16, GL_DEPTH_COMPONENT);
	// m_ShadowMap->SetFilter(Renderer::E_TextureFilter::Linear, Renderer::E_TextureFilter::Linear);
	// m_ShadowMap->SetWrap(Renderer::E_WrapFunction::ClampToBorder, Renderer::E_WrapFunction::ClampToBorder);
	// m_ShadowMap->unbind();
}

//=================================================================================
Renderer::Handle<Renderer::Texture> C_GLAreaLight::GetShadowMap() const
{
	return m_ShadowMap;
}

} // namespace GLEngine::GLRenderer