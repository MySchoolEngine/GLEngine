#include <GLRendererStdafx.h>

#include <GLRenderer/MainPassTechnique.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>

#include <Renderer/IRenderer.h>
#include <Renderer/IRenderableComponent.h>

#include <Entity/IEntity.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_MainPassTechnique::C_MainPassTechnique(std::shared_ptr<Entity::C_EntityManager> world)
	: m_WorldToRender(world)
{
	m_FrameConstUBO = Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Buffers::UBO::C_FrameConstantsBuffer>("frameConst");
}

//=================================================================================
void C_MainPassTechnique::Render(std::shared_ptr<Renderer::I_CameraComponent> camera, unsigned int widht, unsigned int height)
{
	const auto entitiesInView = m_WorldToRender->GetEntities(camera->GetFrustum());

	auto& renderer = (Core::C_Application::Get()).GetActiveRenderer();

	m_FrameConstUBO->SetView(camera->GetViewMatrix());
	m_FrameConstUBO->SetProjection(camera->GetProjectionMatrix());
	m_FrameConstUBO->SetCameraPosition(glm::vec4(camera->GetPosition(), 1.0f));

	{
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

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					m_FrameConstUBO->UploadData();
					m_FrameConstUBO->Activate(true);
				}
				)
		)
	);

	for (auto& entity : entitiesInView)
	{
		if (auto renderable = entity->GetComponent<Entity::E_ComponentType::Graphical>()) {
			renderable->PerformDraw();
		}
	}
}

}