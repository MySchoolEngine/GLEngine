#include <RendererStdafx.h>

#include <Renderer/Lights/AreaLight.h>

namespace GLEngine::Renderer {

//=================================================================================
C_AreaLight::C_AreaLight(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_Light(owner)
	, m_Position(0.f, 0.f, -3.f)
	, m_Normal(glm::normalize(glm::vec3(0, -1.0, 1.0)))
	, m_UpVector(glm::normalize(glm::vec3(0, 1.0, 1.0)))
	, m_Height(10.f)
	, m_Width(10.f) {}

//=================================================================================
C_AreaLight::~C_AreaLight() = default;

//=================================================================================
Physics::Primitives::C_Frustum C_AreaLight::GetShadingFrustum() const
{
	Physics::Primitives::C_Frustum ret(m_Position, m_UpVector, m_Normal, 0.1f, 50.f, 1.0f, 0.f);
	return ret;
}

}