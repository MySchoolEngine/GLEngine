#include <RendererStdafx.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Light/ILight.h>
#include <Renderer/RayCasting/Light/RayAreaLight.h>
#include <Renderer/RayCasting/PhysicalProperties.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/RayCasting/ReflectionModels/LambertianModel.h>
#include <Renderer/RayCasting/ReflectionModels/SpecularReflection.h>
#include <Renderer/RayCasting/Sampling.h>
#include <Renderer/RayCasting/SceneGeometry.h>
#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureStorage.h>
#include <Renderer/Textures/TextureView.h>

#include <glm/gtx/component_wise.hpp>

namespace GLEngine::Renderer {

//=================================================================================
C_RayRenderer::C_RayRenderer(const C_RayTraceScene& scene)
	: m_Scene(scene)
	, m_ProcessedPixels(0)
{
	Textures::TextureLoader tl;
	m_Texture = tl.loadTexture(R"(Models\Bricks01\REGULAR\1K\Bricks01_COL_VAR2_1K.bmp)");
}

//=================================================================================
C_RayRenderer::~C_RayRenderer()
{
	delete m_Texture;
}

//=================================================================================
void C_RayRenderer::Render(I_CameraComponent& camera, I_TextureViewStorage& storage)
{
	m_ProcessedPixels = 0;

	const auto dim	 = storage.GetDimensions();

	C_STDSampler rnd(0.f, 1.f);

	const auto GetRay = [&](const glm::vec2& screenCoord) {
		const float x = (2.0f * screenCoord.x) / dim.x - 1.0f;
		const float y = 1.0f - (2.0f * screenCoord.y) / dim.y;
		return camera.GetRay({x, y});
	};

	auto textureView = C_TextureView(&storage);

	C_TextureView brickView(m_Texture);

	for (int y = 0; y < dim.y; ++y)
	{
		for (int x = 0; x < dim.x; ++x)
		{
			const auto ray = GetRay(glm::vec2{x, y} + rnd.GetV2());
			AddSample({x, y}, textureView, PathTrace(ray, rnd));
			++m_ProcessedPixels;
		}
	}
}

//=================================================================================
void C_RayRenderer::AddSample(const glm::ivec2 coord, C_TextureView view, const glm::vec3 sample)
{
	view.Set(coord, view.Get<glm::vec3>(coord) + sample);
}

//=================================================================================
glm::vec3 C_RayRenderer::PathTrace(const Physics::Primitives::S_Ray& ray, C_STDSampler& rnd)
{
	const float	  eps = 1e-3f;
	C_TextureView brickView(m_Texture);

	C_RayIntersection intersect;

	// first primary ray
	if (!m_Scene.Intersect(ray, intersect))
		return glm::vec3(0.f); // here we can plug environmental light/atmosphere/whatever

	// direct ray to the light intersection
	if (intersect.IsLight())
	{
		auto light = intersect.GetLight();
		return light->Le();
	}

	glm::vec3 LoDirect(0.f);

	m_Scene.ForEachLight([&](const RayTracing::C_AreaLight& light) {
		glm::vec3 wig;
		float	  pdf, distance;
		glm::vec3 illum = light.SampleLi(intersect, &rnd, wig, &distance, &pdf);

		if (glm::compMax(illum) > 0.f)
		{
			if (glm::dot(wig, intersect.GetFrame().Normal()) <= 0.f)
			{
				return;
			}
			C_RayIntersection		   lightIntersect;
			Physics::Primitives::S_Ray lightRay{intersect.GetIntersectionPoint(), wig};
			if (m_DirectionsView) {
				m_DirectionsView->Set(wig, 1.f, E_TextureChannel::Red);
			}
			if (m_Scene.Intersect(lightRay, lightIntersect, eps)) {
				if (lightIntersect.GetRayLength() < distance - eps) {
					return;
				}
				const auto& point		  = intersect.GetIntersectionPoint();
				const auto* material	  = intersect.GetMaterial();
				auto		diffuseColour = glm::vec3(material->diffuse);
				if (material->textureIndex != 0)
				{
					const auto uv = glm::vec2(point.x, point.z) / 10.f;
					diffuseColour = brickView.Get<glm::vec3, T_Bilinear>(uv);
				}
				LoDirect += illum * (diffuseColour / glm::pi<float>());
			}
		}
	});

	return LoDirect;
}

//=================================================================================
std::size_t C_RayRenderer::GetProcessedPixels() const
{
	return m_ProcessedPixels;
}

} // namespace GLEngine::Renderer