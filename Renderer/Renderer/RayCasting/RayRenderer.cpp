#include <RendererStdafx.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/RayCasting/ReflectionModels/LambertianModel.h>
#include <Renderer/RayCasting/ReflectionModels/SpecularReflection.h>
#include <Renderer/RayCasting/Sampling.h>
#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureStorage.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {

//=================================================================================
C_RayRenderer::C_RayRenderer(const C_RayTraceScene& scene)
	: m_Scene(scene)
{
}

//=================================================================================
void C_RayRenderer::Render(I_CameraComponent& camera, I_TextureViewStorage& storage)
{
	const auto low1	 = 0.0;
	const auto high1 = 5.0f;
	const auto low2	 = 0.0;
	const auto high2 = 255.0;
	const auto dim	 = storage.GetDimensions();

	std::random_device		  rd;
	std::mt19937::result_type seed
		= rd()
		  ^ ((std::mt19937::result_type)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()
			 + (std::mt19937::result_type)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());

	std::mt19937						  gen(seed);
	std::uniform_real_distribution<float> distrib(0, 1.f);

	const auto ToClipSpace = [&](const glm::vec2& screenCoord) -> glm::vec2 {
		const float x = (2.0f * screenCoord.x) / dim.x - 1.0f;
		const float y = 1.0f - (2.0f * screenCoord.y) / dim.y;
		return {x, y};
	};

	const auto GetRay = [&](int x, int y) {
		const auto clipSpace = ToClipSpace({static_cast<float>(x), static_cast<float>(y)});
		return camera.GetRay(clipSpace);
	};

	auto textureView = Renderer::C_TextureView(&storage);

	const glm::vec3								   lightNormal = glm::normalize(glm::vec3(-1, -0.5f, 0));
	const C_Primitive<Physics::Primitives::S_Disc> areaLight(Physics::Primitives::S_Disc(lightNormal, glm::vec3(0, 1, 0), 1.f));

	Textures::TextureLoader tl;
	const auto				textureBuffer = tl.loadTexture(R"(Models\Bricks01\REGULAR\1K\Bricks01_COL_VAR2_1K.bmp)");
	C_TextureView			view(textureBuffer);

	const auto areLightPower = 10.f;

	for (int y = 0; y < dim.y; ++y)
	{
		for (int x = 0; x < dim.x; ++x)
		{
			const auto		  ray = GetRay(x, y);
			C_RayIntersection intersect;

			// direct ray to the light intersection
			if (areaLight.Intersect(ray, intersect))
			{
				textureView.Set({x, y}, textureView.Get<glm::vec3>(glm::ivec2{x, y}) + glm::vec3{areLightPower, areLightPower, areLightPower});
				continue;
			}

			// first primary ray
			if (!m_Scene.Intersect(ray, intersect))
				continue; // here we can plug environmental light/atmosphere/whatever

			const auto	point = intersect.GetIntersectionPoint();

			const auto& frame = intersect.GetFrame();

			C_RayIntersection intersectLight;
			const auto		  wol = frame.ToLocal(-ray.direction);
			glm::vec3		  wil;
			float			  pdf;
			C_LambertianModel planeMaterial(glm::vec3(intersect.GetMaterial()->diffuse));
			planeMaterial.SampleF(wol, wil, frame, {distrib(gen), distrib(gen)}, &pdf);
			const Physics::Primitives::S_Ray lightRay{point, frame.ToWorld(wil)};

			if (m_DirectionsView)
			{
				m_DirectionsView->Set(wil, 1.f, E_TextureChannel::Red);
			}

			if (!areaLight.Intersect(lightRay, intersectLight))
			{
				continue;
			}

			C_RayIntersection geomIntersect;
			if (m_Scene.Intersect(lightRay, geomIntersect, std::numeric_limits<float>::epsilon()))
			{
				// light missed, here we should continue the ray
				continue;
			}

			// ===========================
			// here we know, that we hit light and it is not occluded
			// ===========================


			const auto lightDistance = std::powf(intersectLight.GetRayLength(), 2.f);
			const auto cosTheta		 = frame.CosTheta(wil);

			if (cosTheta <= 0.f)
				continue;

			const auto lightFrame = S_Frame(lightNormal);
			const auto cosyTheta  = lightFrame.CosTheta(lightFrame.ToLocal(-lightRay.direction));

			if (cosyTheta <= 0.f)
				continue;

			const auto illum = areLightPower * areaLight.Area() * ((cosTheta * cosyTheta) / lightDistance);

			if (wil.y <= 0 || wol.y <= 0)
				continue;

			auto diffuseColour = intersect.GetMaterial()->diffuse;
			if (glm::abs(intersect.GetIntersectionPoint().y) <= std::numeric_limits<float>::epsilon())
			{
				const auto uv = glm::vec2(point.x, point.z) / 10.f;
				diffuseColour = view.Get<glm::vec4, T_Bilinear>(uv);
			}

			const auto diffuseComponent = diffuseColour / glm::pi<float>();

			const auto sample = (glm::vec3(diffuseComponent) * illum) / pdf;

			textureView.Set({x, y}, textureView.Get<glm::vec3>(glm::ivec2{x, y}) + sample);
		}
	}
}

} // namespace GLEngine::Renderer