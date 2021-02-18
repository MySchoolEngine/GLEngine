#include <RendererStdafx.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Light/ILight.h>
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

	C_STDSampler rnd(0.f, 1.f);

	const auto GetRay = [&](const glm::vec2& screenCoord) {
		const float x = (2.0f * screenCoord.x) / dim.x - 1.0f;
		const float y = 1.0f - (2.0f * screenCoord.y) / dim.y;
		return camera.GetRay({x, y});
	};

	auto textureView = C_TextureView(&storage);

	Textures::TextureLoader tl;
	const auto				textureBuffer = tl.loadTexture(R"(Models\Bricks01\REGULAR\1K\Bricks01_COL_VAR2_1K.bmp)");
	C_TextureView			view(textureBuffer);

	for (int y = 0; y < dim.y; ++y)
	{
		for (int x = 0; x < dim.x; ++x)
		{
			const auto		  ray = GetRay(glm::vec2{x, y} + rnd.GetV2());
			C_RayIntersection intersect;

			// first primary ray
			if (!m_Scene.Intersect(ray, intersect))
				continue; // here we can plug environmental light/atmosphere/whatever

			// direct ray to the light intersection
			if (intersect.IsLight())
			{
				auto light = intersect.GetLight();
				AddSample({x, y}, textureView, light->Le());
				continue;
			}

			const auto point = intersect.GetIntersectionPoint();

			const auto& frame = intersect.GetFrame();

			C_RayIntersection intersectLight;
			const auto		  wol = frame.ToLocal(-ray.direction);
			glm::vec3		  wil;
			float			  pdf;
			if (glm::abs(point.x + 3.f) <= std::numeric_limits<float>::epsilon())
			{
				C_SpecularReflection wallMaterial(PhysicalProperties::IOR::Air, PhysicalProperties::IOR::Glass);
				wallMaterial.SampleF(wol, wil, frame, rnd.GetV2(), &pdf);

				if (m_DirectionsView)
				{
					m_DirectionsView->Set(wil, 1.f, E_TextureChannel::Red);
				}
			}
			else
			{
				C_LambertianModel planeMaterial(glm::vec3(intersect.GetMaterial()->diffuse));
				planeMaterial.SampleF(wol, wil, frame, rnd.GetV2(), &pdf);
			}
			const Physics::Primitives::S_Ray lightRay{point, frame.ToWorld(wil)};

			if (!m_Scene.Intersect(lightRay, intersectLight, std::numeric_limits<float>::epsilon()))
			{
				continue; // complete miss of scene
			}
			// light missed, here we should continue the ray
			if (!intersectLight.IsLight())
				continue;

			// ===========================
			// here we know, that we hit light and it is not occluded
			// ===========================


			const auto lightDistance = std::powf(intersectLight.GetRayLength(), 2.f);
			const auto cosTheta		 = frame.CosTheta(wil);

			if (cosTheta <= 0.f)
				continue;

			const auto& lightFrame = intersectLight.GetFrame();
			const auto	cosyTheta  = lightFrame.CosTheta(lightFrame.ToLocal(-lightRay.direction));

			if (cosyTheta <= 0.f)
				continue;

			const auto illum = intersectLight.GetLight()->Le() * ((cosTheta * cosyTheta) / lightDistance);

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

			AddSample({x, y}, textureView, sample);
		}
	}
}

//=================================================================================
void C_RayRenderer::AddSample(const glm::ivec2 coord, C_TextureView view, const glm::vec3 sample)
{
	view.Set(coord, view.Get<glm::vec3>(coord) + sample);
}

} // namespace GLEngine::Renderer