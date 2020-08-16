#include <GLRendererStdafx.h>

#include <GLRenderer/Lights/GLAreaLight.h>

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_GLAreaLight::C_GLAreaLight(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::C_AreaLight(owner)
	, m_ShadowMap(Textures::C_TextureManager::Instance().CreateEmptyTexture(owner->GetName() + "_ShadowMap"))
	, m_Pos("Light position", m_Position)
	, m_WidthSlider(m_Width, 0.1f, 10.f, "Width")
	, m_HeightSlider(m_Height, 0.1f, 10.f, "Height")
	, m_DiffuseColor("Diffuse colour", glm::vec3(1.f))
	, m_SpecularColor("Spec colour", glm::vec3(1.f))
{
	m_ShadowMap->bind();
	m_ShadowMap->SetDimensions({ 512,512 });
	m_ShadowMap->SetInternalFormat(GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_FLOAT);
	m_ShadowMap->SetFilter(GL_LINEAR, GL_LINEAR);
	m_ShadowMap->SetWrap(E_WrapFunction::ClampToBorder, E_WrapFunction::ClampToBorder);
	m_ShadowMap->unbind();
}

//=================================================================================
const std::shared_ptr<Textures::C_Texture>& C_GLAreaLight::GetShadowMap() const
{
	return m_ShadowMap;
}

//=================================================================================
GLEngine::Physics::Primitives::C_Frustum C_GLAreaLight::GetShadingFrustum() const
{
	Physics::Primitives::C_Frustum ret(m_Pos.GetValue(), m_UpVector, m_Normal, 0.1f, 50.f, 1.0f, 0.f);
	return ret;
}

//=================================================================================
void C_GLAreaLight::DebugDrawGUI()
{
	if (::ImGui::CollapsingHeader("AreaLight")) {
		m_Pos.Draw();
		m_WidthSlider.Draw();
		m_HeightSlider.Draw();
		m_DiffuseColor.Draw();
		m_SpecularColor.Draw();
	}
}

//=================================================================================
void C_GLAreaLight::DebugDraw() const
{
	const auto dirX = glm::cross(m_Normal, m_UpVector);
	const auto width = std::sqrt(GetWidth() / 2.0f);
	const auto height = std::sqrt(GetHeight() / 2.0f);

	C_DebugDraw::Instance().DrawLine(m_Pos.GetValue(), m_Pos.GetValue() + m_Normal, glm::vec3(1.f, 1.f, 0.f));

	C_DebugDraw::Instance().DrawLine(m_Pos.GetValue() + m_UpVector * height + dirX * width, m_Pos.GetValue() + m_UpVector * height - dirX * width, glm::vec3(1.f, 1.f, 0.f));
	C_DebugDraw::Instance().DrawLine(m_Pos.GetValue() - m_UpVector * height + dirX * width, m_Pos.GetValue() - m_UpVector * height - dirX * width, glm::vec3(1.f, 1.f, 0.f));

	C_DebugDraw::Instance().DrawPoint(m_Pos.GetValue() + m_UpVector * height + dirX * width, glm::vec3(0,1,0));

	C_DebugDraw::Instance().DrawLine(m_Pos.GetValue() - m_UpVector * height + dirX * width, m_Pos.GetValue() + m_UpVector * height + dirX * width, glm::vec3(1.f, 1.f, 0.f));
	C_DebugDraw::Instance().DrawLine(m_Pos.GetValue() - m_UpVector * height - dirX * width, m_Pos.GetValue() + m_UpVector * height - dirX * width, glm::vec3(1.f, 1.f, 0.f));
}

//=================================================================================
float C_GLAreaLight::GetWidth() const
{
	return m_WidthSlider.GetValue();
}

//=================================================================================
float C_GLAreaLight::GetHeight() const
{
	return m_HeightSlider.GetValue();
}

//=================================================================================
glm::vec3 C_GLAreaLight::DiffuseColour() const
{
	return m_DiffuseColor.GetValue();
}

//=================================================================================
glm::vec3 C_GLAreaLight::SpecularColour() const
{
	return m_SpecularColor.GetValue();
}

}