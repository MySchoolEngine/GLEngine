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
C_AreaLight::C_AreaLight(const glm::vec3& radiance, const std::shared_ptr<C_Primitive<Physics::Primitives::S_Disc>>& shape)
	: m_Radiance(radiance)
	, m_Shape(shape)
{
}

//=================================================================================
// sampleIllumination in PGIII
glm::vec3 C_AreaLight::SampleLi(const C_RayIntersection& intersection, I_Sampler* rnd, S_VisibilityTester& vis, float* pdf) const
{
	const auto ligthRadius = m_Shape->m_Primitive.radius;
	const auto samplePoint = ligthRadius * SampleConcentricDisc(rnd->GetV2());
	const auto lightFrame  = S_Frame(m_Shape->m_Primitive.plane.normal);
	const auto lightPoint  = RayTracing::T_GeometryTraits::SamplePoint(m_Shape->m_Primitive, rnd);

	vis = S_VisibilityTester(lightPoint, intersection.GetIntersectionPoint());

	auto	   wi		= lightPoint - intersection.GetIntersectionPoint();
	const auto distSqr	= glm::length2(wi);
	auto	   distance = glm::sqrt(distSqr);
	wi /= distance;

	const auto cosThetaX = glm::dot(intersection.GetFrame().Normal(), wi);
	const auto cosThetaY = glm::dot(lightFrame.Normal(), -wi);

	if (cosThetaX <= 0.f || cosThetaY <= 0.f)
		return glm::vec3(0.f);

	const auto areaInv = 1.f / (ligthRadius * ligthRadius * glm::pi<float>());

	*pdf = (distSqr / cosThetaY) * areaInv;

	return Le() * (cosThetaX * cosThetaY) / (distSqr * areaInv);
}

//=================================================================================
glm::vec3 C_AreaLight::Le() const
{
	return m_Radiance;
}

//=================================================================================
std::shared_ptr<I_RayGeometryObject> C_AreaLight::GetGeometry() const
{
	return m_Shape;
}

} // namespace GLEngine::Renderer::RayTracing
