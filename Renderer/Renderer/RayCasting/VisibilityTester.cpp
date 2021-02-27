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
	const auto						 direction = m_End - m_Start;
	const auto						 distance  = glm::distance(m_Start, m_End);
	const Physics::Primitives::S_Ray ray{m_Start, direction / distance};

	C_RayIntersection intersect;

	if (!scene.Intersect(ray, intersect))
		return true;

	return intersect.GetRayLength() > distance - eps;
}

} // namespace GLEngine::Renderer::RayTracing