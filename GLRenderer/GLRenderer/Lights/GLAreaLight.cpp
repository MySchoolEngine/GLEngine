#include <GLRendererStdafx.h>

#include <GLRenderer/Lights/GLAreaLight.h>

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_GLAreaLight::C_GLAreaLight(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::C_AreaLight(owner)
	, m_ShadowMap(Textures::C_TextureManager::Instance().CreateEmptyTexture(owner->GetName() + "_ShadowMap"))
{
	m_ShadowMap->bind();
	m_ShadowMap->SetDimensions({ 512,512 });
	m_ShadowMap->SetInternalFormat(GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_FLOAT);
	m_ShadowMap->SetFilter(GL_LINEAR, GL_LINEAR);
	m_ShadowMap->unbind();
}

//=================================================================================
const std::shared_ptr<Textures::C_Texture>& C_GLAreaLight::GetShadowMap() const
{
	return m_ShadowMap;
}

}