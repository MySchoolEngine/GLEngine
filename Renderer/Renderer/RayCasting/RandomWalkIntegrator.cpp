#include <RendererStdafx.h>

#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/RandomWalkIntegrator.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/ReflectionModels/IReflectionModel.h>
#include <Renderer/RayCasting/Sampling.h>

namespace GLEngine::Renderer {

//=================================================================================
RandomWalkIntegrator::RandomWalkIntegrator(const IntegratorSettings& settings)
	: I_Integrator(settings.Scene)
	, m_MaxDepth(settings.MaxDepth)
{
}

//=================================================================================
Colours::T_Colour RandomWalkIntegrator::TraceRay(Physics::Primitives::S_Ray ray, I_Sampler& rnd)
{
	RayTracingSettings::T_ReflAlloc alloc;
	return LiRandomWalk(ray, rnd, 0, &alloc);
}

//=================================================================================
Colours::T_Colour RandomWalkIntegrator::LiRandomWalk(Physics::Primitives::S_Ray ray, I_Sampler& rnd, unsigned int depth, RayTracingSettings::T_ReflAlloc* alloc)
{
	C_RayIntersection intersect;
	// Intersect with scene
	if (!m_Scene.Intersect(ray, intersect, 1e-3f))
	{
		glm::vec3 LoDirect(0.f);
		m_Scene.ForEachInfiniteLight([&LoDirect](const std::reference_wrapper<const RayTracing::I_RayLight>& light) { LoDirect += light.get().Le(); });
		return LoDirect;
	}

	// todo here goes emittance materials

	glm::vec3 LoDirect(0.f);
	if (intersect.IsLight())
	{
		const auto& light = intersect.GetLight();
		LoDirect += light->Lo(intersect.GetIntersectionPoint(), intersect.GetFrame().Normal(), intersect.GetUV(), -ray.direction);
	}

	if (depth == m_MaxDepth)
	{
		return LoDirect;
	}

	const auto& frame	 = intersect.GetFrame();
	const auto* material = intersect.GetMaterial();

	const auto model = material->GetScatteringFunction(intersect, *alloc);

	const auto wol = frame.ToLocal(-ray.direction);

	// sample sphere for direction

	glm::vec3 wi = UniformSampleSphere(rnd.GetV2());

	const auto fcos = model->f(wol, wi) * std::abs(glm::dot(wi, frame.Normal()));
	if (fcos == Colours::black)
	{
		return LoDirect;
	}

	const auto newRay = intersect.SpawnRay(wi);
	return LoDirect + fcos * LiRandomWalk(newRay, rnd, depth + 1, alloc) / (4 * glm::one_over_pi<float>());
}
} // namespace GLEngine::Renderer