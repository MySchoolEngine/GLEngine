#include <RendererStdafx.h>

#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/VisibilityTester.h>

#include <Physics/Primitives/Ray.h>

namespace GLEngine::Renderer::RayTracing {

//=================================================================================
S_VisibilityTester::S_VisibilityTester(const glm::vec3& start, const glm::vec3& end)
	: m_Start(start)
	, m_End(end)
{
}

//=================================================================================
bool S_VisibilityTester::IsVisible(const C_RayTraceScene& scene, float eps) const
{
	const auto						 distance = glm::distance(m_Start, m_End);
	const Physics::Primitives::S_Ray ray	  = GetRay();

	C_RayIntersection intersect;

	if (!scene.Intersect(ray, intersect))
		return true;

	return intersect.GetRayLength() > distance - eps;
}

//=================================================================================
Physics::Primitives::S_Ray S_VisibilityTester::GetRay() const
{
	const auto direction = m_End - m_Start;
	const auto distance	 = glm::distance(m_Start, m_End);
	return {m_Start, direction / distance};
}

} // namespace GLEngine::Renderer::RayTracing