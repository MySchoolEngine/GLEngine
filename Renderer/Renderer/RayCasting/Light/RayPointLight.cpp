#include <RendererStdafx.h>

#include <Renderer/RayCasting/Light/RayPointLight.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/VisibilityTester.h>
#include <Renderer/Colours.h>

#include <glm/gtx/norm.hpp>

namespace GLEngine::Renderer::RayTracing {

//=================================================================================
C_PointLight::C_PointLight(const glm::vec3& position, const glm::vec3& intenstiy)
	: m_Position(position)
	, m_Intensity(intenstiy)
{
}

//=================================================================================
Colours::T_Colour C_PointLight::SampleLi(const C_RayIntersection& intersection, I_Sampler* rnd, S_VisibilityTester& vis, float* pdf) const
{
	auto	   wi		= m_Position - intersection.GetIntersectionPoint();
	float	   distSqr	= glm::length2(wi);
	const auto distance = glm::sqrt(distSqr);

	vis = S_VisibilityTester(intersection.GetIntersectionPoint(), m_Position);

	wi = wi / distance;

	const auto costTheta = glm::dot(intersection.GetFrame().Normal(), wi);

	if (costTheta <= 0.f)
		return glm::vec3(0.f);

	*pdf = 1.f;

	return m_Intensity * costTheta / distSqr;
}

//=================================================================================
Colours::T_Colour C_PointLight::Le() const
{
	return m_Intensity;
}

} // namespace GLEngine::Renderer::RayTracing
