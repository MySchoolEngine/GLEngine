#include <RendererStdafx.h>

#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/ReflectionModels/IReflectionModel.h>
#include <Renderer/RayCasting/Sampling.h>
#include <Renderer/RayCasting/SimplePathIntegrator.h>


namespace GLEngine::Renderer::RayTracing {
//=================================================================================
SimplePathIntegrator::SimplePathIntegrator(const IntegratorSettings& settings)
	: I_Integrator(settings.Scene)
	, m_MaxDepth(settings.MaxDepth)
	, m_SampleLights(settings.SampleLights)
	, m_SampleBRDF(settings.SampleBRDF)
{
}

//=================================================================================
Colours::T_Colour SimplePathIntegrator::TraceRay(Physics::Primitives::S_Ray ray, I_Sampler& rnd)
{
	RayTracingSettings::T_ReflAlloc alloc;
	Colours::T_Colour				Li	  = Colours::black;
	Colours::T_Colour				beta  = Colours::white;
	unsigned int					depth = 0;
	while (beta != Colours::black)
	{
		// Intersect with scene
		C_RayIntersection intersect;
		if (!m_Scene.Intersect(ray, intersect, 1e-3f))
		{
			if (!m_SampleLights)
				m_Scene.ForEachInfiniteLight([&Li, &beta](const std::reference_wrapper<const RayTracing::I_RayLight>& light) { Li += beta * light.get().Le(); });
			break;
		}

		// TODO emissive surface

		if (intersect.IsLight())
		{
			const auto& light = intersect.GetLight();
			Li += beta * light->Lo(intersect.GetIntersectionPoint(), intersect.GetFrame().Normal(), intersect.GetUV(), -ray.direction);
		}

		if (depth++ == m_MaxDepth)
		{
			break;
		}

		const auto& frame	 = intersect.GetFrame();
		const auto* material = intersect.GetMaterial();

		const auto brdf = material->GetScatteringFunction(intersect, alloc);

		const auto wol = frame.ToLocal(-ray.direction);

		if (m_SampleLights)
		{
			// uniform light sampling
			// todo better lights heuristic
			// const int lightIndex = std::min<int>(rnd.GetD() * m_Scene.GetNumLights(), m_Scene.GetNumLights() - 1);
		}

		if (m_SampleBRDF)
		{
			// sample BRDF for new path
			glm::vec3  wi;
			float	   pdf;
			const auto f = brdf->SampleF(wol, wi, frame, rnd.GetV2(), &pdf);
			beta *= f * std::abs(glm::dot(wi, frame.Normal())) / pdf;
			// something with specular bounce
			ray = intersect.SpawnRay(wi);
		}
		else
		{
			// Uniform sphere sample
			// todo for reflective surfaces use only hemisphere
			glm::vec3 wi = UniformSampleSphere(rnd.GetV2());
			const auto fcos = brdf->f(wol, wi) * std::abs(glm::dot(wi, frame.Normal())) / UniformSpherePDF();
			beta *= fcos;
			// something with specular bounce
			ray = intersect.SpawnRay(wi);
		}
	}
	return Li;
}
} // namespace GLEngine::Renderer::RayTracing