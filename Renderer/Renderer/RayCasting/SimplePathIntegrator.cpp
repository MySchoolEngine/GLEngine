#include <RendererStdafx.h>

#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/ReflectionModels/IReflectionModel.h>
#include <Renderer/RayCasting/Sampling.h>
#include <Renderer/RayCasting/SimplePathIntegrator.h>
#include <Renderer/RayCasting/VisibilityTester.h>


namespace GLEngine::Renderer::RayTracing {
//=================================================================================
SimplePathIntegrator::SimplePathIntegrator(const IntegratorSettings& settings)
	: I_Integrator(settings.Scene)
	, m_MaxDepth(settings.MaxDepth)
	, m_SampleLights(settings.SampleLights)
	, m_SampleBRDF(settings.SampleBRDF)
	, m_LightSampler(nullptr)
{
	auto lights	   = m_Scene.GetLights();
	m_LightSampler = std::make_unique<UniformLightSample>(lights);
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
				m_Scene.ForEachInfiniteLight([&Li, &beta](const std::reference_wrapper<const I_RayLight>& light) { Li += beta * light.get().Le(); });
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
			auto sampledLight = m_LightSampler->SampleLight(rnd);
			if (sampledLight)
			{
				auto	   vis = S_VisibilityTester(glm::vec3(), glm::vec3());
				float	   lightPdf;
				const auto illum = sampledLight->m_Light.SampleLi(intersect, rnd, vis, &lightPdf);
				if (illum != Colours::black && lightPdf > 0.f)
				{
					const Colours::T_Colour f = brdf->f(wol, frame.ToLocal(vis.GetRay().direction)) * S_Frame::AbsCosTheta(frame.ToLocal(vis.GetRay().direction));
					if (f != Colours::black && vis.IsVisible(m_Scene))
					{
						Li += beta * f * illum / (sampledLight->pdf * lightPdf);
					}
				}
			}
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
			glm::vec3 wi;
			float	  pdf;
			if (brdf->GetType().CheckFlag(I_ReflectionModel::Type::Transmission))
			{
				wi	= UniformSampleSphere(rnd.GetV2());
				pdf = UniformSpherePDF();
			}
			else
			{
				wi	= UniformSampleHemisphere(rnd.GetV2());
				pdf = UniformHemispherePDF();
			}
			const auto fcos = brdf->f(wol, wi) * S_Frame::AbsCosTheta(wi) / pdf;
			beta *= fcos;
			// something with specular bounce
			ray = intersect.SpawnRay(wi);
		}
	}
	return Li;
}
} // namespace GLEngine::Renderer::RayTracing