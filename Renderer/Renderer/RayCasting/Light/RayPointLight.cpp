#include <RendererStdAfx.h>

#include <Renderer/RayCasting/Light/RayPointLight.h>
#include <Renderer/RayCasting/RayIntersection.h>

#include <glm/gtx/norm.hpp>

namespace GLEngine::Renderer::RayTracing {

//=================================================================================
C_PointLight::C_PointLight(const glm::vec3& position, const glm::vec3& intenstiy)
	: m_Position(position)
	, m_Intensity(intenstiy)
{
}

//=================================================================================
glm::vec3 C_PointLight::SampleLi(const C_RayIntersection& intersection, I_Sampler* rnd, glm::vec3& wi, float* distance, float* pdf) const
{
	wi			  = m_Position - intersection.GetIntersectionPoint();
	float distSqr = glm::length2(wi);
	*distance	  = glm::sqrt(distSqr);

	wi = wi / (*distance);

	const auto costTheta = glm::dot(intersection.GetFrame().Normal(), wi);

	if (costTheta <= 0.f)
		return glm::vec3(0.f);

	*pdf = 1.f;

	return m_Intensity * costTheta / distSqr;
}

//=================================================================================
glm::vec3 C_PointLight::Le() const
{
	return m_Intensity;
}

//=================================================================================
float C_PointLight::PdfLe() const
{
	return 1.f;
}

} // namespace GLEngine::Renderer::RayTracing
