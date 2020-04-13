#include <GLRendererStdafx.h>

#include <GLRenderer/MainPassTechnique.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Debug.h>

#include <GLRenderer/Lights/LightsUBO.h>

#include <Renderer/IRenderer.h>
#include <Renderer/IRenderableComponent.h>
#include <Renderer/ILight.h>
#include <Renderer/Lights/PointLight.h>

#include <Entity/IEntity.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_MainPassTechnique::C_MainPassTechnique(std::shared_ptr<Entity::C_EntityManager> world)
	: m_WorldToRender(world)
	, m_SunX(-13.f, -20.f, 20.f, "Sun X")
	, m_SunY(15.f, -20.f, 20.f, "Sun Y")
	, m_SunZ(-5.f, -20.f, 20.f, "Sun Z")
{
	m_FrameConstUBO = Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Buffers::UBO::C_FrameConstantsBuffer>("frameConst");
	m_LightsUBO			= Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<C_LightsBuffer>("lightsUni");
}

//=================================================================================
void C_MainPassTechnique::Render(std::shared_ptr<Renderer::I_CameraComponent> camera, unsigned int widht, unsigned int height)
{
	RenderDoc::C_DebugScope s("C_MainPassTechnique::Render");
	const auto entitiesInView = m_WorldToRender->GetEntities(camera->GetFrustum());

	auto& renderer = (Core::C_Application::Get()).GetActiveRenderer();

	m_FrameConstUBO->SetView(camera->GetViewMatrix());
	m_FrameConstUBO->SetProjection(camera->GetProjectionMatrix());
	m_FrameConstUBO->SetCameraPosition(glm::vec4(camera->GetPosition(), 1.0f));
	m_FrameConstUBO->SetSunPosition({ m_SunX.GetValue(), m_SunY.GetValue(), m_SunZ.GetValue() });

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
	}

	for (auto& entity : entitiesInView)
	{
		if (auto light = entity->GetComponent<Entity::E_ComponentType::Light>()) {
			const auto pointLight = std::reinterpret_pointer_cast<Renderer::I_PointLight>(light);
			if (pointLight)
			{
				const auto pos = pointLight->GetPosition();

				S_PointLight pl;
				pl.m_Position = pos;
				pl.m_Color = pointLight->GetColor();
				pl.m_Intensity = pointLight->GetIntensity();

				m_LightsUBO->SetPointLight(pl);

				C_DebugDraw::Instance().DrawPoint(glm::vec4(pos, 1.0), glm::vec3(1, 1, 1));
			}
		}
	}

	{
		RenderDoc::C_DebugScope s("UBO Upload");
		Core::C_Application::Get().GetActiveRenderer()->AddCommand(
			std::move(
				std::make_unique<Commands::HACK::C_LambdaCommand>(
					[&]() {
						m_FrameConstUBO->UploadData();
						m_FrameConstUBO->Activate(true);
						m_LightsUBO->UploadData();
						m_LightsUBO->Activate(true);
					}
					)
			)
		);
	}

	for (auto& entity : entitiesInView)
	{
		if (auto renderable = entity->GetComponent<Entity::E_ComponentType::Graphical>()) {
			renderable->PerformDraw();
		}
	}

	C_DebugDraw::Instance().DrawPoint({ m_SunX.GetValue(), m_SunY.GetValue(), m_SunZ.GetValue() }, {1.f, 1.f, 0.f});
	bool my_tool_active = true;
	::ImGui::Begin("Sun", &my_tool_active);
		m_SunX.Draw();
		m_SunY.Draw();
		m_SunZ.Draw();
	::ImGui::End();
}

}