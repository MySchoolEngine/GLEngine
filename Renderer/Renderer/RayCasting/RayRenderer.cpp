#include <RendererStdafx.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Geometry/SceneGeometry.h>
#include <Renderer/RayCasting/Light/ILight.h>
#include <Renderer/RayCasting/Light/RayAreaLight.h>
#include <Renderer/RayCasting/PhysicalProperties.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/RayCasting/ReflectionModels/LambertianModel.h>
#include <Renderer/RayCasting/ReflectionModels/SpecularReflection.h>
#include <Renderer/RayCasting/Sampling.h>
#include <Renderer/RayCasting/VisibilityTester.h>
#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureStorage.h>
#include <Renderer/Textures/TextureView.h>

#include <glm/gtx/component_wise.hpp>

namespace GLEngine::Renderer {

//=================================================================================
C_RayRenderer::C_RayRenderer(const C_RayTraceScene& scene)
	: m_Scene(scene)
	, m_ProcessedPixels(0)
	, m_MaxDepth(3)
	, m_NewResultAviable(false)
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
void C_RayRenderer::Render(I_CameraComponent& camera, I_TextureViewStorage& weightedImage, I_TextureViewStorage& storage, std::mutex* storageMutex, int numSamplesBefore)
{
	const auto dim	  = storage.GetDimensions();
	m_ProcessedPixels = 0;


	C_STDSampler rnd(0.f, 1.f);

	const auto GetRay = [&](const glm::vec2& screenCoord) {
		const float x = (2.0f * screenCoord.x) / dim.x - 1.0f;
		const float y = 1.0f - (2.0f * screenCoord.y) / dim.y;
		return camera.GetRay({x, y});
	};

	auto textureView = C_TextureView(&storage);

	int interleavedLines = 2;

	for (int y = 0; y < dim.y; y += interleavedLines)
	{
		for (int x = 0; x < dim.x; ++x)
		{
			const auto ray = GetRay(glm::vec2{x, y} + rnd.GetV2());
			AddSample({x, y}, textureView, PathTrace(ray, rnd));
			++m_ProcessedPixels;
		}

		auto weightedView = C_TextureView(&weightedImage);
		if (storageMutex)
		{
			std::lock_guard<std::mutex> lock(*storageMutex);
			UpdateView(y, interleavedLines, textureView, weightedView, numSamplesBefore + 1);
		}
		else
		{
			UpdateView(y, interleavedLines, textureView, weightedView, numSamplesBefore + 1);
		}
	}
	for (int y = interleavedLines / 2; y < dim.y; y += interleavedLines)
	{
		for (int x = 0; x < dim.x; ++x)
		{
			const auto ray = GetRay(glm::vec2{x, y} + rnd.GetV2());
			AddSample({x, y}, textureView, PathTrace(ray, rnd));
			++m_ProcessedPixels;
		}

		auto weightedView = C_TextureView(&weightedImage);
		if (storageMutex)
		{
			std::lock_guard<std::mutex> lock(*storageMutex);
			UpdateView(y, interleavedLines / 2, textureView, weightedView, numSamplesBefore + 1);
		}
		else
		{
			UpdateView(y, interleavedLines / 2, textureView, weightedView, numSamplesBefore + 1);
		}
	}
}

//=================================================================================
void C_RayRenderer::UpdateView(unsigned int sourceLine, unsigned int numLines, C_TextureView& source, C_TextureView& target, unsigned int numSamples)
{
	const auto dim = target.GetDimensions();
	for (int x = 0; x < dim.x; ++x)
	{
		const auto denominator = 1.0f / static_cast<float>(numSamples);
		const auto sourceLineVal = source.Get<glm::vec3>(glm::ivec2{x, sourceLine}) * denominator;
		for (int i = sourceLine; i < std::min(dim.y, sourceLine + numLines); ++i)
		{
			const auto previousLineVal = source.Get<glm::vec3>(glm::ivec2{x, i}) * (1.0f - denominator);
			target.Set({x, i}, (sourceLineVal + previousLineVal) * denominator);
		}
	}
	m_NewResultAviable = true;
}

//=================================================================================
void C_RayRenderer::AddSample(const glm::ivec2 coord, C_TextureView view, const glm::vec3 sample)
{
	const auto previousValue = view.Get<glm::vec3>(coord);
	view.Set(coord, previousValue + sample);
}

//=================================================================================
glm::vec3 C_RayRenderer::PathTrace(Physics::Primitives::S_Ray ray, C_STDSampler& rnd)
{
	C_TextureView brickView(m_Texture);
	glm::vec3	  LoDirect(0.f);
	glm::vec3	  throughput(1.f);

	for (std::size_t pathDepth = 0; pathDepth < m_MaxDepth; ++pathDepth)
	{
		C_RayIntersection intersect, intersectY;
		// first primary ray
		if (!m_Scene.Intersect(ray, intersect, 1e-3f))
			break;

		// direct ray to the light intersection
		if (intersect.IsLight())
		{
			auto light = intersect.GetLight();
			LoDirect += throughput * light->Le();
		}

		const auto& frame		  = intersect.GetFrame();
		const auto& point		  = intersect.GetIntersectionPoint();
		const auto* material	  = intersect.GetMaterial();
		auto		diffuseColour = glm::vec3(material->diffuse);
		if (material->textureIndex != 0)
		{
			const auto uv = glm::vec2(point.x, point.z) / 10.f;
			diffuseColour = brickView.Get<glm::vec3, T_Bilinear>(uv);
		}
		C_LambertianModel model(diffuseColour);

		const auto wol = frame.ToLocal(-ray.direction);

		glm::vec3  wi;
		float	   pdf;
		const auto f = model.SampleF(wol, wi, frame, rnd.GetV2(), &pdf);

		GLE_ASSERT(wi.y > 0, "Wrong direction of the ray!");

		Physics::Primitives::S_Ray rayY;
		rayY.origin	   = point;
		rayY.direction = frame.ToWorld(wi);

		if (!m_Scene.Intersect(rayY, intersectY, 1e-3f))
			break;

		if (intersectY.IsLight())
		{
			auto light = intersectY.GetLight();
			const auto diffusePart = material->diffuse * glm::one_over_pi<float>();
			const auto lightPart   = wi.y * light->Le() / pdf;
			LoDirect += glm::vec3(diffusePart.x * lightPart.x, diffusePart.y * lightPart.y, diffusePart.z * lightPart.z);
		}
		break;
	}

	return LoDirect;
}

//=================================================================================
glm::vec3 C_RayRenderer::DirectLighting(const Physics::Primitives::S_Ray& ray, C_STDSampler& rnd)
{
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

	m_Scene.ForEachLight([&](const std::reference_wrapper<const RayTracing::I_RayLight>& light) {
		float	  pdf;
		auto	  vis	= RayTracing::S_VisibilityTester(glm::vec3(), glm::vec3());
		glm::vec3 illum = light.get().SampleLi(intersect, &rnd, vis, &pdf);

		if (glm::compMax(illum) > 0.f)
		{
			if (!vis.IsVisible(m_Scene))
			{
				return;
			}
			// no intersect for point light

			const auto&		  point			= intersect.GetIntersectionPoint();
			const auto*		  material		= intersect.GetMaterial();
			const auto&		  frame			= intersect.GetFrame();
			auto			  diffuseColour = glm::vec3(material->diffuse);
			C_LambertianModel model(diffuseColour);
			if (material->textureIndex != 0)
			{
				const auto uv = glm::vec2(point.x, point.z) / 10.f;
				diffuseColour = brickView.Get<glm::vec3, T_Bilinear>(uv);
			}
			LoDirect += illum * model.f(frame.ToLocal(ray.direction), frame.ToLocal(vis.GetRay().direction));
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