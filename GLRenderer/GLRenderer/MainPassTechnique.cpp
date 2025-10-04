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

#include <Renderer/Components/StaticMeshHandles.h>
#include <Renderer/DebugDraw.h>
#include <Renderer/ICameraComponent.h>
#include <Renderer/IRenderableComponent.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Lights/AreaLight.h>
#include <Renderer/Lights/PointLight.h>
#include <Renderer/Lights/SunLight.h>
#include <Renderer/Materials/MaterialManager.h>

#include <Physics/Primitives/Frustum.h>

#include <Entity/EntityManager.h>
#include <Entity/IEntity.h>

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
void C_MainPassTechnique::Render(const Entity::C_EntityManager&				  world,
								 std::shared_ptr<Renderer::I_CameraComponent> camera,
								 unsigned int								  width,
								 unsigned int								  height,
								 Renderer::I_DebugDraw&						  dd,
								 Renderer::I_RenderInterface3D&				  renderInterface3D)
{
	RenderDoc::C_DebugScope s("C_MainPassTechnique::Render");
	const auto				camFrustum	   = camera->GetFrustum();
	const auto				camBox		   = camFrustum.GetAABB().GetSphere();
	const auto				entitiesInView = world.GetEntities(camFrustum);

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
	for (auto& entity : entitiesInView)
	{
		for (const auto& lightIt : entity->GetComponents(Entity::E_ComponentType::Light))
		{
			const auto pointLight = std::dynamic_pointer_cast<Renderer::C_PointLight>(lightIt);
			if (pointLight && pointLightIndex < m_LightsUBO->PointLightsLimit())
			{
				const auto pos = pointLight->GetPosition();

				S_PointLight pl;
				pl.m_Position  = pos;
				pl.m_Color	   = pointLight->GetColor();
				pl.m_Intensity = pointLight->GetIntensity();

				m_LightsUBO->SetPointLight(pl, pointLightIndex);
				++pointLightIndex;

				dd.DrawPoint(glm::vec4(pos, 1.0), pointLight->GetColor());
			}

			const auto areaLight = std::dynamic_pointer_cast<Renderer::C_AreaLight>(lightIt);
			if (areaLight && areaLightIndex < m_LightsUBO->AreaLightsLimit())
			{
				// auto& tm = Textures::C_TextureUnitManger::Instance();
				// tm.BindTextureToUnit(*areaLight->GetShadowMap(), 5 + static_cast<unsigned int>(areaLightIndex));
				const auto frustum = areaLight->GetShadingFrustum();


				const auto pos	  = frustum.GetPosition();
				const auto up	  = frustum.GetUpVector();
				const auto width  = areaLight->GetWidth() / 2.0f;
				const auto height = areaLight->GetHeight() / 2.0f;

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
				light.m_Color		  = areaLight->DiffuseColour();
				light.m_SpecularColor = areaLight->SpecularColour();
				light.m_Intensity	  = 1.f;

				areaLight->DebugDraw(&dd);
				m_LightsUBO->SetAreaLight(light, areaLightIndex);
				++areaLightIndex;
			}

			const auto sunLight = std::dynamic_pointer_cast<Renderer::C_SunLight>(lightIt);
			if (sunLight)
			{
				auto& glRM			 = dynamic_cast<C_OGLRenderer&>(renderer).GetRMGR();
				auto* sunShadowMapGL = glRM.GetTexture(m_SunShadowMap);
				m_LightsUBO->GetSunLight().SetSunPosition(sunLight->GetSunDirection());
				m_LightsUBO->GetSunLight().m_SunColor			 = sunLight->GetSunColor();
				m_LightsUBO->GetSunLight().m_AsymmetricFactor	 = sunLight->AtmosphereAsymetricFactor();
				m_LightsUBO->GetSunLight().m_SunDiscMultiplier	 = sunLight->SunDiscMultiplier();
				m_LightsUBO->GetSunLight().m_LightViewProjection = m_SunViewProjection;
				m_LightsUBO->GetSunLight().m_SunShadowMap		 = sunShadowMapGL->GetHandle();
				m_LightsUBO->GetSunLight().m_SunlightPresent	 = 1;
				sunLightFound									 = true;

				dd.DrawPoint(sunLight->GetSunDirection(), Colours::yellow);
				dd.DrawLine({0.f, 0.f, 0.f}, sunLight->GetSunDirection(), Colours::yellow);
			}
		}
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
			[&, materialsHaveChanged]() {
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
		for (auto& entity : entitiesInView)
		{
			auto renderableComponentsRange = entity->GetComponents(Entity::E_ComponentType::Graphical);
			for (const auto& it : renderableComponentsRange)
			{
				const auto renderableComp = component_cast<Entity::E_ComponentType::Graphical>(it);
				const auto compSphere	  = renderableComp->GetAABB().GetSphere();
				if (compSphere.IsColliding(camBox))
					component_cast<Entity::E_ComponentType::Graphical>(it)->PerformDraw();

				const auto staticMeshHandles = std::dynamic_pointer_cast<Renderer::C_StaticMeshHandles>(it);
				if (staticMeshHandles)
				{
					staticMeshHandles->Render(m_3DRenderer);
				}
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