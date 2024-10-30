#include <RendererStdafx.h>

#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/Integrator.h>
#include <Renderer/RayCasting/Light/BackgroundLight.h>
#include <Renderer/RayCasting/Light/ILight.h>
#include <Renderer/RayCasting/PhysicalProperties.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/ReflectionModels/LambertianModel.h>
#include <Renderer/RayCasting/ReflectionModels/OrenNayar.h>
#include <Renderer/RayCasting/ReflectionModels/SpecularReflection.h>
#include <Renderer/RayCasting/VisibilityTester.h>

#include <glm/gtx/component_wise.hpp>

namespace GLEngine::Renderer {

//=================================================================================
C_PathIntegrator::C_PathIntegrator(const C_RayTraceScene& scene)
	: m_Scene(scene)
{
}

//=================================================================================
Colours::T_Colour C_PathIntegrator::TraceRay(Physics::Primitives::S_Ray ray, I_Sampler& rnd)
{
	RayTracingSettings::T_ReflAlloc alloc;
	return Li_PathTrace(ray, rnd, &alloc);
}

//=================================================================================
Colours::T_Colour C_PathIntegrator::Li_Direct(const Physics::Primitives::S_Ray& ray, I_Sampler& rnd, RayTracingSettings::T_ReflAlloc* alloc)
{
	// this is basing the direction on BRDF somehow
	glm::vec3 LoDirect(0.f);

	C_RayIntersection intersect, intersectY;
	// first primary ray
	if (!m_Scene.Intersect(ray, intersect, 1e-3f))
		return Colours::black;

	// direct ray to the light intersection
	if (intersect.IsLight())
	{
		auto light = intersect.GetLight();
		LoDirect += light->Le();
	}

	const auto& frame	 = intersect.GetFrame();
	const auto& point	 = intersect.GetIntersectionPoint();
	const auto* material = intersect.GetMaterial();
	const auto	model	 = material->GetScatteringFunction(intersect, *alloc);

	const auto wol = frame.ToLocal(-ray.direction);

	glm::vec3  wi;
	float	   pdf;
	const auto f = model->SampleF(wol, wi, frame, rnd.GetV2(), &pdf);

	GLE_ASSERT(wi.y > 0, "Wrong direction of the ray!");

	Physics::Primitives::S_Ray rayY;
	rayY.origin	   = point;
	rayY.direction = frame.ToWorld(wi);

	if (!m_Scene.Intersect(rayY, intersectY, 1e-3f))
		return LoDirect;

	if (intersectY.IsLight())
	{
		auto	   light	 = intersectY.GetLight();
		const auto lightPart = wi.y * light->Le() / pdf;
		LoDirect += glm::vec3(f.x * lightPart.x, f.y * lightPart.y, f.z * lightPart.z);
	}

	return LoDirect;
}

//=================================================================================
Colours::T_Colour C_PathIntegrator::Li_PathTrace(Physics::Primitives::S_Ray ray, I_Sampler& rnd, RayTracingSettings::T_ReflAlloc* alloc)
{
	Colours::T_Colour LoDirect	 = Colours::black; // f in his example
	Colours::T_Colour throughput = Colours::white;

	C_RayIntersection intersect;
	// first primary ray

	RayTracing::C_BackgroundLight bgLight(glm::vec3(135, 206, 250) / glm::vec3(255.f));

	int i = 0;

	while (true)
	{
		if (!m_Scene.Intersect(ray, intersect, 1e-3f))
		{
			LoDirect += throughput * bgLight.Le();
			break;
		}

		if (i == 0)
		{
			if (intersect.IsLight())
			{
				auto light = intersect.GetLight();
				LoDirect += throughput * light->Le();
			}
		}

		if (i >= 15)
			break;

		const auto& frame	 = intersect.GetFrame();
		const auto& point	 = intersect.GetIntersectionPoint();
		const auto* material = intersect.GetMaterial();

		const auto model = material->GetScatteringFunction(intersect, *alloc);

		// add Li
		Colours::T_Colour Li = Li_LightSampling(ray, rnd, alloc);
		LoDirect += Li * throughput;

		const auto wol = frame.ToLocal(-ray.direction);

		glm::vec3  wi;
		float	   pdf;
		const auto f = model->SampleF(wol, wi, frame, rnd.GetV2(), &pdf);

		if (f == glm::vec3(0, 0, 0) || pdf == 0.f)
			break;

		throughput *= f * wi.y / pdf;

		GLE_ASSERT(wi.y >= 0, "Wrong direction of the ray!");

		// russian roulette after first 3 bounces!
		if (i > 3)
		{
			const auto	survProb = std::min(1.f, glm::compMax(throughput));
			const float random	 = rnd.GetD();
			if (random >= survProb)
				break;

			throughput /= survProb;
		}

		// generate new ray
		ray.origin	  = intersect.GetIntersectionPoint();
		ray.direction = frame.ToWorld(wi);
		++i;
	}

	return LoDirect;
}

//=================================================================================
Colours::T_Colour C_PathIntegrator::Li_LightSampling(const Physics::Primitives::S_Ray& ray, I_Sampler& rnd, RayTracingSettings::T_ReflAlloc* alloc)
{
	// this simply samples from all the lights
	C_RayIntersection intersect;

	// first primary ray
	if (!m_Scene.Intersect(ray, intersect))
		return Colours::black; // here we can plug environmental light/atmosphere/whatever

	Colours::T_Colour LoDirect(0.f);

	// direct ray to the light intersection
	if (intersect.IsLight())
	{
		auto light = intersect.GetLight();
		LoDirect += light->Le();
	}

	m_Scene.ForEachLight([&](const std::reference_wrapper<const RayTracing::I_RayLight>& light) {
		LoDirect += EstimateDirect(intersect, light, rnd, alloc);
		// float			  pdf;
		// auto			  vis	= RayTracing::S_VisibilityTester(glm::vec3(), glm::vec3());
		// Colours::T_Colour illum = light.get().SampleLi(intersect, rnd, vis, &pdf);
		//
		// if (glm::compMax(illum) > 0.f)
		//{
		//	if (!vis.IsVisible(m_Scene))
		//	{
		//		return;
		//	}
		//	// no intersect for point light
		//
		//	const auto& point	 = intersect.GetIntersectionPoint();
		//	const auto* material = intersect.GetMaterial();
		//	const auto& frame	 = intersect.GetFrame();
		//	const auto	model	 = material->GetScatteringFunction(intersect, *alloc);
		//	LoDirect += illum * model->f(frame.ToLocal(ray.direction), frame.ToLocal(vis.GetRay().direction));
		//}
	});

	return LoDirect;
}

inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf)
{
	float f = nf * fPdf, g = ng * gPdf;
	return (f * f) / (f * f + g * g);
}

Colours::T_Colour
C_PathIntegrator::EstimateDirect(const C_RayIntersection& intersection, const RayTracing::I_RayLight& light, I_Sampler& rnd, RayTracingSettings::T_ReflAlloc* alloc)
{
	Colours::T_Colour LoDirect = Colours::black;
	glm::vec3		  wi(0.f);
	float			  lightPdf		= 0;
	float			  scatteringPdf = 0;
	auto			  vis			= RayTracing::S_VisibilityTester(glm::vec3(), glm::vec3());

	const auto& point	 = intersection.GetIntersectionPoint();
	const auto* material = intersection.GetMaterial();
	const auto& frame	 = intersection.GetFrame();
	const auto	model	 = material->GetScatteringFunction(intersection, *alloc); // this should be way before to avoid allocation

	const Colours::T_Colour illum = light.SampleLi(intersection, rnd, vis, &lightPdf);
	if (illum != Colours::black && lightPdf > 0.f)
	{
		Colours::T_Colour f		   = model->SampleF(frame.ToLocal(intersection.GetRay().direction), wi, frame, rnd.GetV2(), &scatteringPdf) * wi.y;

		if (f != Colours::black)
		{
			// here we can handle media
			if (!vis.IsVisible(m_Scene))
			{
				LoDirect = Colours::black;
			}

			if (LoDirect == Colours::black)
			{
				// calculate weight for MIS
				if (light.IsDeltaLight())
				{
					LoDirect += f * illum / lightPdf;
				}
				else
				{
					// delta light is point lights
					float weight = PowerHeuristic(1, lightPdf, 1, scatteringPdf);
					LoDirect += f * illum * model->f(frame.ToLocal(intersection.GetRay().direction), frame.ToLocal(vis.GetRay().direction)) * weight / lightPdf;
				}
			}
		}
	}

	// BRDF sampling here
	if (!light.IsDeltaLight())
	{ // we cant hit delta lights
		Colours::T_Colour f = model->SampleF(frame.ToLocal(intersection.GetRay().direction), wi, frame, rnd.GetV2(), &scatteringPdf);
		f *= wi.y;
		if (f != Colours::black && scatteringPdf > 0.f) {
			float weight = 1.f;
			lightPdf = light.Pdf_Li(wi);
			if (lightPdf == 0)
				return LoDirect;
			weight = PowerHeuristic(1, scatteringPdf, 1, lightPdf);

			Colours::T_Colour		   Li = Colours::black;
			C_RayIntersection lightIntersect;
			Physics::Primitives::S_Ray lightLigth(intersection.GetIntersectionPoint(), wi);
			if (m_Scene.Intersect(lightLigth, lightIntersect)) {
				// check if we hit given light
				if (lightIntersect.GetLight().get() == &light) {
					Li = light.Le();// wrong
				}
			}
			if (Li != Colours::black) {
				LoDirect += f * Li * weight / scatteringPdf;
			}
		}
	}

	return LoDirect;
}

} // namespace GLEngine::Renderer
