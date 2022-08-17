#include <RendererStdAfx.h>

#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/Integrator.h>
#include <Renderer/RayCasting/Light/ILight.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/ReflectionModels/LambertianModel.h>
#include <Renderer/RayCasting/ReflectionModels/SpecularReflection.h>
#include <Renderer/RayCasting/VisibilityTester.h>
#include <Renderer/RayCasting/PhysicalProperties.h>

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
	return Li_PathTrace(ray, rnd);
}

//=================================================================================
Colours::T_Colour C_PathIntegrator::Li_Direct(const Physics::Primitives::S_Ray& ray, I_Sampler& rnd)
{
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

	const auto& frame		  = intersect.GetFrame();
	const auto& point		  = intersect.GetIntersectionPoint();
	const auto* material	  = intersect.GetMaterial();
	const auto	uv			  = intersect.GetUV();
	auto		diffuseColour = glm::vec3(material->diffuse);
	if (material->textureIndex != -1)
	{
		diffuseColour = m_Scene.GetTextureView(material->textureIndex).Get<glm::vec3, T_Bilinear>(uv);
	}
	const std::unique_ptr<I_ReflectionModel> model = GetReflectionModel(material, diffuseColour);

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
Colours::T_Colour C_PathIntegrator::Li_PathTrace(Physics::Primitives::S_Ray ray, I_Sampler& rnd)
{
	Colours::T_Colour LoDirect	 = Colours::black; // f in his example
	Colours::T_Colour throughput = Colours::white;

	C_RayIntersection intersect;
	// first primary ray

	int i = 0;

	while (true)
	{
		if (!m_Scene.Intersect(ray, intersect, 1e-3f))
			return LoDirect; // here the background colour goes

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

		const auto& frame		  = intersect.GetFrame();
		const auto& point		  = intersect.GetIntersectionPoint();
		const auto* material	  = intersect.GetMaterial();
		const auto	uv			  = intersect.GetUV();
		auto		diffuseColour = glm::vec3(material->diffuse);
		if (material->textureIndex != -1)
		{
			diffuseColour = m_Scene.GetTextureView(material->textureIndex).Get<glm::vec3, T_Bilinear>(uv);
		}

		const std::unique_ptr<I_ReflectionModel> model = GetReflectionModel(material, diffuseColour);

		// add Li
		Colours::T_Colour Li = Li_LightSampling(ray, rnd);
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
Colours::T_Colour C_PathIntegrator::Li_LightSampling(const Physics::Primitives::S_Ray& ray, I_Sampler& rnd)
{
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
		float			  pdf;
		auto			  vis	= RayTracing::S_VisibilityTester(glm::vec3(), glm::vec3());
		Colours::T_Colour illum = light.get().SampleLi(intersect, &rnd, vis, &pdf);

		if (glm::compMax(illum) > 0.f)
		{
			if (!vis.IsVisible(m_Scene))
			{
				return;
			}
			// no intersect for point light

			const auto& point		  = intersect.GetIntersectionPoint();
			const auto* material	  = intersect.GetMaterial();
			const auto& frame		  = intersect.GetFrame();
			const auto	uv			  = intersect.GetUV();
			auto		diffuseColour = Colours::T_Colour(material->diffuse);
			if (material->textureIndex != -1)
			{
				diffuseColour = m_Scene.GetTextureView(material->textureIndex).Get<glm::vec3, T_Bilinear>(uv);
			}
			const std::unique_ptr<I_ReflectionModel> model = GetReflectionModel(material, diffuseColour);
			LoDirect += illum * model->f(frame.ToLocal(ray.direction), frame.ToLocal(vis.GetRay().direction));
		}
	});

	return LoDirect;
}

//=================================================================================
std::unique_ptr<Renderer::I_ReflectionModel> C_PathIntegrator::GetReflectionModel(const MeshData::Material* material, Colours::T_Colour& colour) const
{
	if (material->shininess == 0.f)
		return std::make_unique<C_LambertianModel>(colour);
	else
		return std::make_unique<C_SpecularReflection>(PhysicalProperties::IOR::Air, PhysicalProperties::IOR::Glass);
}

} // namespace GLEngine::Renderer
