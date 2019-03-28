#include <GLRendererStdafx.h>

#include <GLRenderer/Windows/ExperimentWindow.h>

#include <GLRenderer/Cameras/OrbitalCamera.h>

#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GlClearColor.h>
#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/MeshLoading/SceneLoader.h>
#include <GLRenderer/MeshLoading/Scene.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>

#include <memory>

namespace GLEngine {
namespace GLRenderer {
namespace Windows {

std::shared_ptr<Shaders::C_ShaderProgram> program;

//=================================================================================
C_ExplerimentWindow::C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo)
	: C_GLFWoGLWindow(wndInfo)
{
	program = Shaders::C_ShaderManager::Instance().GetProgram("basic-planes");

	m_OrbitalCamera = std::make_shared<Cameras::C_OrbitalCamera>();
	m_OrbitalCamera->setupCameraProjection(0.1f, 2 * (20.0f), static_cast<float>(wndInfo.m_width)/ static_cast<float>(wndInfo.m_height), 90.0f);
	m_OrbitalCamera->setupCameraView(20.0f, glm::vec3(0.0f),0,0);
	m_OrbitalCamera->update();

	auto sl = std::make_unique<Mesh::SceneLoader>();

	auto scene = std::make_shared<Mesh::Scene>();
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	if (!sl->addModelFromFileToScene("Models", "scene.obj", scene, modelMatrix))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Unable to load model {}", "Models/scene.obj");
		return;
	}

	m_Mesh = std::make_shared<Mesh::C_StaticMeshResource>(scene->meshes[0]);


	m_FrameConstUBO = Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Buffers::UBO::C_FrameConstantsBuffer>("frameConst");
}

//=================================================================================
void C_ExplerimentWindow::Update()
{
	Shaders::C_ShaderManager::Instance().Update();
	auto red = rand() % 255;

	{
		using namespace Commands;
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLClearColor>(glm::vec3(/*static_cast<float>(red) / 255.0f*/ 0.0f, 0.0f, 0.0f))
			)
		);
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLClear>(C_GLClear::E_ClearBits::Color | C_GLClear::E_ClearBits::Stencil)
			)
		);
		m_renderer->AddCommand(
			std::move(
				std::make_unique<HACK::C_DrawStaticMesh>(m_Mesh)
			)
		);
	}

	glfwMakeContextCurrent(m_Window);

	Shaders::C_ShaderManager::Instance().ActivateShader(program);

	program->SetUniform("modelMatrix", glm::mat4(1.0f));
	program->SetUniform("modelColor", glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
	program->SetUniform("hasTexture", false);

	m_OrbitalCamera->adjustOrientation(0.1f, 0.1f);
	m_OrbitalCamera->update();

	glCullFace(GL_FRONT_AND_BACK);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 640, 480);

	m_FrameConstUBO->SetViewProjection(m_OrbitalCamera->GetViewProjectionMatrix());
	m_FrameConstUBO->SetCameraPosition(glm::vec4(m_OrbitalCamera->GetPosition(), 1.0f));
	m_FrameConstUBO->UploadData();
	m_FrameConstUBO->Activate(true);

	m_renderer->Commit();

	m_renderer->ClearCommandBuffers();

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

}
}
}