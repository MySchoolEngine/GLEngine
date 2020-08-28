#include <RendererStdafx.h>

#include <Renderer/Lights/AreaLight.h>

namespace GLEngine::Renderer {

//=================================================================================
C_AreaLight::C_AreaLight(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_Light(owner)
	, m_Normal(glm::normalize(glm::vec3(0, -1.0, 1.0)))
	, m_UpVector(glm::normalize(glm::vec3(0, 1.0, 1.0)))
	, m_Height(10.f)
	, m_Width(10.f) {}

//=================================================================================
C_AreaLight::~C_AreaLight() = default;

//=================================================================================
Physics::Primitives::C_Frustum C_AreaLight::GetShadingFrustum() const
{
	Physics::Primitives::C_Frustum ret(m_ComponentMatrix[3], m_UpVector, m_Normal, 0.1f, 50.f, 1.0f, 0.f);
	return ret;
}

//=================================================================================
Physics::Primitives::S_AABB C_AreaLight::GetAABB() const
{
	Physics::Primitives::S_AABB aabb;

	const auto dirX = glm::cross(m_Normal, m_UpVector);
	const auto dirY = m_UpVector;

	const auto width = std::sqrt(GetWidth() / 2.0f);
	const auto height = std::sqrt(GetHeight() / 2.0f);

	aabb.Add(m_ComponentMatrix * (glm::vec4(+ dirY * height + dirX * width, 1.f)));
	aabb.Add(m_ComponentMatrix * (glm::vec4(+ dirY * height - dirX * width, 1.f)));
	aabb.Add(m_ComponentMatrix * (glm::vec4(- dirY * height + dirX * width, 1.f)));
	aabb.Add(m_ComponentMatrix * (glm::vec4(- dirY * height - dirX * width, 1.f)));

	return aabb;
}

}