#include <RendererStdAfx.h>

#include <Renderer/RayCasting/Light/RayAreaLight.h>
#include <Renderer/RayCasting/SceneGeometry.h>

namespace GLEngine::Renderer::RayTracing {

//=================================================================================
C_AreaLight::C_AreaLight(const glm::vec3& radiance, const std::shared_ptr<I_RayGeometryObject>& shape)
	: m_Radiance(radiance)
	, m_Shape(shape)
{
}

//=================================================================================
glm::vec3 C_AreaLight::SampleLi() const
{
	return m_Radiance;
}

//=================================================================================
glm::vec3 C_AreaLight::Le() const
{
	return m_Radiance;
}

//=================================================================================
float C_AreaLight::PdfLe() const
{
	return 0.f;
}

//=================================================================================
std::shared_ptr<I_RayGeometryObject> C_AreaLight::GetGeometry() const
{
	return m_Shape;
}

} // namespace GLEngine::Renderer::RayTracing
