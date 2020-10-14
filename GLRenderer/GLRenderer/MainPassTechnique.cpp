#include <GLRendererStdafx.h>

#include <GLRenderer/MainPassTechnique.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Lights/GLAreaLight.h>
#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Textures/TextureUnitManager.h>
#include <GLRenderer/Debug.h>
#include <GLRenderer/OGLRenderer.h>

#include <GLRenderer/Lights/LightsUBO.h>

#include <Renderer/IRenderer.h>
#include <Renderer/IRenderableComponent.h>
#include <Renderer/ICameraComponent.h>
#include <Renderer/ILight.h>
#include <Renderer/Lights/PointLight.h>

#include <Entity/IEntity.h>
#include <Entity/EntityManager.h>

#include <Core/Application.h>

#include <glm/gtc/matrix_transform.hpp>

namespace GLEngine::GLRenderer {

//=================================================================================
C_MainPassTechnique::C_MainPassTechnique(std::shared_ptr<Entity::C_EntityManager> world)
	: m_WorldToRender(world)
	, m_SunX(0.f, -1.f, 1.f, "Sun X")
	, m_SunY(1.f, -1.f, 1.f, "Sun Y")
	, m_SunZ(0.f, -1.f, 1.f, "Sun Z")
{
	m_FrameConstUBO = Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Buffers::UBO::C_FrameConstantsBuffer>("frameConst");
	m_LightsUBO			= Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<C_LightsBuffer>("lightsUni");
}

//=================================================================================
void C_MainPassTechnique::Render(std::shared_ptr<Renderer::I_CameraComponent> camera, unsigned int widht, unsigned int height)
{
	RenderDoc::C_DebugScope s("C_MainPassTechnique::Render");
	const auto camFrustum = camera->GetFrustum();
	const auto camBox = camFrustum.GetAABB().GetSphere();
	const auto entitiesInView = m_WorldToRender->GetEntities(camFrustum);

	auto& renderer = (Core::C_Application::Get()).GetActiveRenderer();
	renderer->SetCurrentPassType(Renderer::E_PassType::FinalPass);

	m_FrameConstUBO->SetView(camera->GetViewMatrix());
	m_FrameConstUBO->SetProjection(camera->GetProjectionMatrix());
	m_FrameConstUBO->SetCameraPosition(glm::vec4(camera->GetPosition(), 1.0f));
	m_FrameConstUBO->SetNearPlane(camera->GetNear());
	m_FrameConstUBO->SetFarPlane(camera->GetFar());
	m_FrameConstUBO->SetSunPosition({ m_SunX.GetValue(), m_SunY.GetValue(), m_SunZ.GetValue() });
	m_FrameConstUBO->SetFrameTime(static_cast<float>(glfwGetTime()));

	{
		RenderDoc::C_DebugScope s("Window prepare");
		using namespace Commands;
		renderer->AddCommand(
			std::move(
				std::make_unique<C_GLClear>(C_GLClear::E_ClearBits::Color | C_GLClear::E_ClearBits::Depth)
			)
		);
		renderer->AddCommand(
			std::move(
				std::make_unique<C_GLViewport>(0, 0, widht, height)
			)
		);
		if (static_cast<C_OGLRenderer*>(renderer.get())->WantWireframe())
		{
			renderer->AddCommand(
				std::move(
					std::make_unique<Commands::HACK::C_LambdaCommand>(
						[&]() {
							glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						}
						, "Change polygon mode"
					)
				)
			);
		}
	}

	std::size_t pointLightIndex = 0;
	std::size_t areaLightIndex = 0;
	for (auto& entity : entitiesInView)
	{
		for (const auto& lightIt : entity->GetComponents(Entity::E_ComponentType::Light))
		{
			const auto pointLight = std::dynamic_pointer_cast<Renderer::I_PointLight>(lightIt);
			if (pointLight && pointLightIndex < m_LightsUBO->PointLightsLimit())
			{
				const auto pos = pointLight->GetPosition();

				S_PointLight pl;
				pl.m_Position = pos;
				pl.m_Color = pointLight->GetColor();
				pl.m_Intensity = pointLight->GetIntensity();

				m_LightsUBO->SetPointLight(pl, pointLightIndex);
				++pointLightIndex;

				C_DebugDraw::Instance().DrawPoint(glm::vec4(pos, 1.0), pointLight->GetColor());
			}

			const auto areaLight = std::dynamic_pointer_cast<C_GLAreaLight>(lightIt);
			if (areaLight && areaLightIndex < m_LightsUBO->AreaLightsLimit())
			{
				auto& tm = Textures::C_TextureUnitManger::Instance();
				tm.BindTextureToUnit(*areaLight->GetShadowMap(), 5 + static_cast<unsigned int>(areaLightIndex));
				const auto frustum = areaLight->GetShadingFrustum();


				const auto left = glm::normalize(glm::cross(frustum.GetForeward(), frustum.GetUpVector()));
				const auto pos = frustum.GetPosition();
				const auto up = frustum.GetUpVector();
				const auto width = areaLight->GetWidth() / 2.0f;
				const auto height = areaLight->GetHeight() / 2.0f;

				const auto dirX = glm::cross(frustum.GetForeward(), up);

				S_AreaLight light;
				light.m_LightMat = glm::ortho(-width, width, -height, height, frustum.GetNear(), frustum.GetFar()) * glm::lookAt(pos, pos + frustum.GetForeward(), up);
				light.m_Pos = pos;
				light.m_ShadowMap = static_cast<int>(areaLightIndex);
				light.m_Width = width;
				light.m_Height = height;
				light.m_Normal = frustum.GetForeward();
				light.m_DirY = up;
				light.m_DirX = dirX;
				light.m_Color = areaLight->DiffuseColour();
				light.m_SpecularColor = areaLight->SpecularColour();

				C_DebugDraw::Instance().DrawAxis(pos, frustum.GetUpVector(), frustum.GetForeward());
				areaLight->DebugDraw();
				m_LightsUBO->SetAreaLight(light, areaLightIndex);
				++areaLightIndex;
			}
		}
	}

	{
		RenderDoc::C_DebugScope s("UBO Upload");
		m_LightsUBO->MakeHandlesResident();
		renderer->AddCommand(
			std::move(
				std::make_unique<Commands::HACK::C_LambdaCommand>(
					[&]() {
						m_FrameConstUBO->UploadData();
						m_FrameConstUBO->Activate(true);
						m_LightsUBO->UploadData();
						m_LightsUBO->Activate(true);
					}, "MainPass - upload UBOs"
				)
			)
		);
	}

	{
		RenderDoc::C_DebugScope s("Commit geometry");
		for (auto& entity : entitiesInView)
		{
			auto renderableComponentsRange = entity->GetComponents(Entity::E_ComponentType::Graphical);
			for (const auto& it : renderableComponentsRange)
			{
				const auto rendarebleComp = component_cast<Entity::E_ComponentType::Graphical>(it);
				const auto compSphere = rendarebleComp->GetAABB().GetSphere();
				if(compSphere.IsColliding(camBox))
					component_cast<Entity::E_ComponentType::Graphical>(it)->PerformDraw();
			}
		}
	}

	{
		RenderDoc::C_DebugScope s("Clean");
		m_LightsUBO->MakeHandlesResident(false);

		if (static_cast<C_OGLRenderer*>(renderer.get())->WantWireframe())
		{
			renderer->AddCommand(
				std::move(
					std::make_unique<Commands::HACK::C_LambdaCommand>(
						[&]() {
							glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						}, "Reset polygon mode"
					)
				)
			);
		}
	}

	C_DebugDraw::Instance().DrawPoint({ m_SunX.GetValue(), m_SunY.GetValue(), m_SunZ.GetValue() }, { 1.f, 1.f, 0.f });
	C_DebugDraw::Instance().DrawLine({0.f, 0.f, 0.f}, { m_SunX.GetValue(), m_SunY.GetValue(), m_SunZ.GetValue() }, { 1.f, 1.f, 0.f });
	bool my_tool_active = true;
	::ImGui::Begin("Sun", &my_tool_active);
		m_SunX.Draw();
		m_SunY.Draw();
		m_SunZ.Draw();
	::ImGui::End();
}

}