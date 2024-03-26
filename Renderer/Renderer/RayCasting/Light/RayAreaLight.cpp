#include <RendererStdafx.h>

#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Geometry/GeometryTraits.h>
#include <Renderer/RayCasting/Geometry/SceneGeometry.h>
#include <Renderer/RayCasting/Light/RayAreaLight.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/Sampling.h>
#include <Renderer/RayCasting/VisibilityTester.h>

#include <glm/gtx/norm.hpp>

namespace GLEngine::Renderer::RayTracing {

//=================================================================================
// sampleIllumination in PGIII
Colours::T_Colour C_AreaLight::SampleLi(const C_RayIntersection& intersection, I_Sampler& rnd, S_VisibilityTester& vis, float* pdf) const
{
	// const auto samplePoint = ligthRadius * SampleConcentricDisc(rnd->GetV2()); //< TODO for future use when Li is dependant on the position on the light
	const auto lightFrame = S_Frame(m_Shape->Normal());
	const auto lightPoint = m_Shape->SamplePoint(rnd);

	vis = S_VisibilityTester(lightPoint, intersection.GetIntersectionPoint());

	auto	   wi		= lightPoint - intersection.GetIntersectionPoint();
	const auto distSqr	= glm::length2(wi);
	auto	   distance = glm::sqrt(distSqr);
	wi /= distance;

	const auto cosThetaX = glm::dot(intersection.GetFrame().Normal(), wi);
	const auto cosThetaY = glm::dot(lightFrame.Normal(), -wi);

	if (cosThetaX <= 0.f || cosThetaY <= 0.f)
		return Colours::black;

	const auto areaInv = 1.f / m_Shape->Area();

	if (pdf)
		*pdf = (distSqr / cosThetaY) * areaInv;

	return Le() * (cosThetaX * cosThetaY) / (distSqr * areaInv);
}

//=================================================================================
Colours::T_Colour C_AreaLight::Le() const
{
	return m_Radiance;
}

//=================================================================================
float C_AreaLight::Pdf_Li(const glm::vec3& wi) const
{
	const auto areaInv = 1.f / m_Shape->Area();
	const auto distSqr = glm::length2(wi);
	const auto lightFrame = S_Frame(m_Shape->Normal());
	const auto cosThetaY  = glm::dot(lightFrame.Normal(), -wi);
	return (distSqr / cosThetaY) * areaInv;
}

//=================================================================================
std::shared_ptr<I_RayGeometryObject> C_AreaLight::GetGeometry() const
{
	return m_Shape->GetGeometry();
}

} // namespace GLEngine::Renderer::RayTracing
