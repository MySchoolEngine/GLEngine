#include <RendererStdafx.h>

#include <Renderer/RayCasting/RayRenderer.h>

#include <Renderer/RayCasting/Sampling.h>
#include <Renderer/RayCasting/ReflectionModels/SpecularReflection.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/Textures/TextureStorage.h>
#include <Renderer/Textures/TextureView.h>

#include <random>
#include <chrono>

namespace GLEngine::Renderer {

//=================================================================================
C_RayRenderer::C_RayRenderer(const C_RayTraceScene& scene)
	: m_Scene(scene)
{

}

//=================================================================================
void C_RayRenderer::Render(I_CameraComponent& camera, I_TextureViewStorage& storage)
{
	const auto low1 = 0.0;
	const auto high1 = 5.0f;
	const auto low2 = 0.0;
	const auto high2 = 255.0;
	const auto dim = storage.GetDimensions();

	std::random_device rd;
	std::mt19937::result_type seed = rd() ^ (
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
			).count() +
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count());

	std::mt19937 gen(seed);
	std::uniform_real_distribution<float> distrib(0, 1.f);

	const auto ToClipSpace = [&](const glm::vec2& screenCoord) -> glm::vec2
	{
		const float x = (2.0f * screenCoord.x) / dim.x - 1.0f;
		const float y = 1.0f - (2.0f * screenCoord.y) / dim.y;
		return { x, y };
	};

	const auto GetRay = [&](int x, int y) {
		const auto clipSpace = ToClipSpace({ static_cast<float>(x), static_cast<float>(y) });
		return camera.GetRay(clipSpace);
	};

	auto textureView = Renderer::C_TextureView(&storage);

	const auto eps = 1e-5f;
	auto min = std::numeric_limits<float>::max();
	auto max = std::numeric_limits<float>::min();

	const glm::vec3 lightNormal = glm::normalize(glm::vec3(0, -0.5f, -1));
	const C_Primitive<Physics::Primitives::S_Disc> areaLight(Physics::Primitives::S_Disc(lightNormal, glm::vec3(0, 1, 2), .25f));

	const auto areLightPower = 255.f;
	const auto IORGlass = 1.5f;
	const C_SpecularReflection planeMaterial(1.00029f, IORGlass);

	for (int y = 0; y < dim.y; ++y)
	{
		for (int x = 0; x < dim.x; ++x)
		{
			const auto ray = GetRay(x, y);
			C_RayIntersection intersect;

			if (areaLight.Intersect(ray, intersect))
			{
				textureView.Set({ x,y }, textureView.Get<glm::vec3>({ x,y }) + glm::vec3{ areLightPower ,areLightPower ,areLightPower });
				continue;
			}

			if (!m_Scene.Intersect(ray, intersect)) continue;

			const auto& frame = intersect.GetFrame();

			C_RayIntersection intersectLight;
			const auto wol = frame.ToLocal(-ray.direction);
			glm::vec3 wil;
			float pdf;
			planeMaterial.SampleF(-ray.direction, wil, frame, { distrib(gen), distrib(gen) }, &pdf);
			const Physics::Primitives::S_Ray lightRay{ intersect.GetIntersectionPoint(), frame.ToWorld(wil)};

			if (m_DirectionsView)
			{
				m_DirectionsView->Set(wil, 1.f, E_TextureChannel::Red);
			}

			if (!areaLight.Intersect(lightRay, intersectLight))
			{
				continue;
			}

			C_RayIntersection geomIntersect;
			if (m_Scene.Intersect(lightRay, geomIntersect))
			{
				// light missed, here we should continue the ray
				continue;
			}

			// here we know, that we hit light and it is not occluded


			const auto lightDistance = std::powf(intersectLight.GetRayLength(), 2.f);
			const auto cosTheta = frame.CosTheta(wil);

			if (cosTheta <= 0.f)
				continue;

			const auto lightFrame = S_Frame(lightNormal);
			const auto cosyTheta = lightFrame.CosTheta(lightFrame.ToLocal(-lightRay.direction));

			if(cosyTheta<=0.f)
				continue;

			const auto illum = areLightPower * areaLight.Area() * ((cosTheta * cosyTheta) / lightDistance);

			if (wil.y <= 0 || wol.y <= 0)
				continue;

			const auto diffuseComponent = intersect.GetMaterial()->diffuse / glm::pi<float>();

			const auto sample = glm::vec3(diffuseComponent) * illum / pdf;

			textureView.Set({ x,y }, textureView.Get<glm::vec3>({ x,y }) + sample); // Map due to texture format

			// need for mapping purposes
			min = std::min(min, sample.x);
			min = std::min(min, sample.y);

			max = std::max(max, sample.x);
			max = std::max(max, sample.y);
		}
	}

	CORE_LOG(E_Level::Debug, E_Context::Render, "{} {}", min, max);
}

}