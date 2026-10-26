#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Lights/LightsUBO.h>
#include <GLRenderer/MainPassTechnique.h>
#include <GLRenderer/Materials/MaterialBuffer.h>
#include <GLRenderer/OGLRenderer.h>

#include <Renderer/Components/Lights/LightComponents.h>
#include <Renderer/Components/Lights/LightFunctions.h>
#include <Renderer/Components/Mesh/GeomComponents.h>
#include <Renderer/Components/Mesh/StaticMeshComponents.h>
#include <Renderer/DebugDraw.h>
#include <Renderer/ICamera.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Systems/GeomSystem.h>
#include <Renderer/Systems/StaticMeshSystem.h>

#include <Physics/Primitives/Frustum.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/World.h>

#include <Core/Application.h>

#include <glm/gtc/matrix_transform.hpp>

namespace GLEngine::GLRenderer {

//=================================================================================
C_MainPassTechnique::C_MainPassTechnique(Renderer::ResourceManager& resourceManager)
{
	m_FrameConstUBO = Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Buffers::UBO::C_FrameConstantsBuffer>(resourceManager, "frameConst");
	m_LightsUBO		= Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<C_LightsBuffer>(resourceManager, "lightsUni");
	m_MaterialsUBO	= Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Material::C_MaterialsBuffer>(resourceManager, "materials");
}

//=================================================================================
void C_MainPassTechnique::Render(Entity::C_World&					 world,
								 std::shared_ptr<Renderer::I_Camera> camera,
								 unsigned int						 width,
								 unsigned int						 height,
								 Renderer::I_DebugDraw&				 dd,
								 Renderer::I_RenderInterface3D&		 renderInterface3D)
{
	RenderDoc::C_DebugScope s("C_MainPassTechnique::Render");
	const auto				camFrustum = camera->GetFrustum();
	const auto				camBox	   = camFrustum.GetAABB().GetSphere();
	auto&					registry   = world.Registry();

	auto& renderer = (Core::C_Application::Get()).GetActiveRenderer();
	renderer.SetCurrentPassType(Renderer::E_PassType::FinalPass);

	// this is going to be graph node
	{
		RenderDoc::C_DebugScope s("Window prepare");
		using namespace Commands;
		renderer.AddCommand(std::make_unique<C_GLClear>(C_GLClear::E_ClearBits::Color | C_GLClear::E_ClearBits::Depth));
		renderer.AddCommand(std::make_unique<C_GLViewport>(Renderer::C_Viewport(0, 0, width, height)));
		if (static_cast<C_OGLRenderer*>(&renderer)->WantWireframe())
		{
			renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([&]() { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }, "Change polygon mode"));
		}
	}

	std::size_t pointLightIndex = 0;
	std::size_t areaLightIndex	= 0;
	bool		sunLightFound	= false;

	for (auto [e, pointLight, worldTransform] : registry.view<Renderer::S_PointLight, Entity::S_WorldTransformComponent>().each())
	{
		if (pointLightIndex >= m_LightsUBO->PointLightsLimit())
			break;

		const auto pos = glm::vec3(worldTransform.world[3]);

		S_PointLight pl;
		pl.m_Position  = pos;
		pl.m_Color	   = pointLight.color;
		pl.m_Intensity = pointLight.intensity;

		m_LightsUBO->SetPointLight(pl, pointLightIndex);
		++pointLightIndex;

		dd.DrawPoint(glm::vec4(pos, 1.0), pointLight.color);
	}

	for (auto [e, areaLight, worldTransform] : registry.view<Renderer::S_AreaLight, Entity::S_WorldTransformComponent>().each())
	{
		if (areaLightIndex >= m_LightsUBO->AreaLightsLimit())
			break;

		// auto& tm = Textures::C_TextureUnitManger::Instance();
		// tm.BindTextureToUnit(*areaLight->GetShadowMap(), 5 + static_cast<unsigned int>(areaLightIndex));
		const auto frustum = Renderer::GetShadingFrustum(areaLight, worldTransform);

		const auto pos	  = frustum.GetPosition();
		const auto up	  = frustum.GetUpVector();
		const auto width  = areaLight.width / 2.0f;
		const auto height = areaLight.height / 2.0f;

		const auto dirX = glm::cross(frustum.GetForward(), up);

		S_AreaLight light;
		light.m_LightMat	  = glm::ortho(-width, width, -height, height, frustum.GetNear(), frustum.GetFar()) * glm::lookAt(pos, pos + frustum.GetForward(), up);
		light.m_Pos			  = pos;
		light.m_ShadowMap	  = static_cast<int>(areaLightIndex);
		light.m_Width		  = width;
		light.m_Height		  = height;
		light.m_Normal		  = frustum.GetForward();
		light.m_DirY		  = up;
		light.m_DirX		  = dirX;
		light.m_Color		  = areaLight.diffuseColor;
		light.m_SpecularColor = areaLight.specularColor;
		light.m_Intensity	  = 1.f;

		Renderer::DebugDraw(areaLight, worldTransform, dd);
		m_LightsUBO->SetAreaLight(light, areaLightIndex);
		++areaLightIndex;
	}

	for (auto [e, sunLight, worldTransform] : registry.view<Renderer::S_SunLight, Entity::S_WorldTransformComponent>().each())
	{
		auto&			glRM		   = dynamic_cast<C_OGLRenderer&>(renderer).GetRMGR();
		auto*			sunShadowMapGL = glRM.GetTexture(m_SunShadowMap);
		const glm::vec3 sunDirection(sunLight.sunX, sunLight.sunY, sunLight.sunZ);
		m_LightsUBO->GetSunLight().SetSunPosition(sunDirection);
		m_LightsUBO->GetSunLight().m_SunColor			 = sunLight.sunColor;
		m_LightsUBO->GetSunLight().m_AsymmetricFactor	 = sunLight.asymmetricFactor;
		m_LightsUBO->GetSunLight().m_SunDiscMultiplier	 = sunLight.sunDiscMultiplier;
		m_LightsUBO->GetSunLight().m_LightViewProjection = m_SunViewProjection;
		m_LightsUBO->GetSunLight().m_SunShadowMap		 = sunShadowMapGL->GetHandle();
		m_LightsUBO->GetSunLight().m_SunlightPresent	 = 1;
		sunLightFound									 = true;

		dd.DrawPoint(sunDirection, Colours::yellow);
		dd.DrawLine({0.f, 0.f, 0.f}, sunDirection, Colours::yellow);
	}

	if (sunLightFound == false)
		m_LightsUBO->GetSunLight().m_SunlightPresent = 0;

	bool materialsHaveChanged = false;
	{
		int i = 0;
		Renderer::C_MaterialManager::Instance().ForEachMaterial([&matUBO = m_MaterialsUBO, &i, &materialsHaveChanged](Renderer::C_Material& material) {
			if (material.IsChanged())
			{
				materialsHaveChanged = true;
				if (matUBO->m_PhongMaterials[i].Update(material))
					material.CleanChangeFlag();
			}
			++i;
		});
	}


	{
		RenderDoc::C_DebugScope s("UBO Upload");
		m_LightsUBO->MakeHandlesResident();
		renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
			[&, materialsHaveChanged, camera]() {
				m_FrameConstUBO->SetView(camera->GetViewMatrix());
				m_FrameConstUBO->SetProjection(camera->GetProjectionMatrix());
				m_FrameConstUBO->SetCameraPosition(glm::vec4(camera->GetPosition(), 1.0f));
				m_FrameConstUBO->SetNearPlane(camera->GetNear());
				m_FrameConstUBO->SetFarPlane(camera->GetFar());
				m_FrameConstUBO->SetFrameTime(static_cast<float>(glfwGetTime()));
				m_FrameConstUBO->UploadData(renderer);
				m_FrameConstUBO->Activate(renderer.GetRM(), true);
				m_LightsUBO->UploadData(renderer);
				m_LightsUBO->Activate(renderer.GetRM(), true);
				if (materialsHaveChanged)
					m_MaterialsUBO->UploadData(renderer);
				m_MaterialsUBO->Activate(renderer.GetRM(), true);
			},
			"MainPass - upload UBOs"));
	}

	{
		RenderDoc::C_DebugScope s("Commit geometry");
		for (auto [e, bounds, worldTransform] : registry.view<Entity::S_LocalBounds, Entity::S_WorldTransformComponent>().each())
		{
			const auto compSphere = bounds.aabb.GetSphere();
			if (!compSphere.IsColliding(camBox))
				continue;

			if (registry.all_of<Renderer::S_StaticMesh, Renderer::S_StaticMeshGPU>(e))
			{
				Renderer::StaticMeshSystem::Render(registry.get<Renderer::S_StaticMesh>(e), registry.get<Renderer::S_StaticMeshGPU>(e), worldTransform, m_3DRenderer);
			}
			else if (registry.all_of<Renderer::S_GeomComponent, Renderer::S_GeomGPU>(e))
			{
				Renderer::GeomSystem::Render(registry.get<Renderer::S_GeomComponent>(e), registry.get<Renderer::S_GeomGPU>(e), worldTransform, m_3DRenderer);
			}
		}
	}

	{
		RenderDoc::C_DebugScope scope3DRenderer("3D Renderer");
		m_3DRenderer.Commit(renderInterface3D);
		m_3DRenderer.Clear();
	}

	{
		RenderDoc::C_DebugScope s("Clean");
		m_LightsUBO->MakeHandlesResident(false);

		if (static_cast<C_OGLRenderer*>(&renderer)->WantWireframe())
		{
			renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([&]() { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }, "Reset polygon mode"));
		}
	}
}

//=================================================================================
void C_MainPassTechnique::SetSunShadowMap(Renderer::Handle<Renderer::Texture> sunShadowMapHandle)
{
	m_SunShadowMap = sunShadowMapHandle;
}

//=================================================================================
void C_MainPassTechnique::SetSunViewProjection(glm::mat4 viewProjection)
{
	m_SunViewProjection = viewProjection;
}

} // namespace GLEngine::GLRenderer