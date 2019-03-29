#include <GLRendererStdafx.h>

#include <GLRenderer/Windows/ExperimentWindow.h>

#include <GLRenderer/Cameras/OrbitalCamera.h>

#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GlClearColor.h>
#include <GLRenderer/Commands/GLCullFace.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>

#include <GLRenderer/Components/StaticMesh.h>

#include <Entity/BasicEntity.h>

#include <memory>
#include <iomanip>
#define ErrorCheck() _glErrorCheck(__FILE__, __LINE__)

//=================================================================================
const char* glErrorCodeToString(unsigned int code) {
#define codeToStr(c) case c: return #c; break
	switch (code)
	{
		codeToStr(GL_INVALID_ENUM);
		codeToStr(GL_INVALID_VALUE);
		codeToStr(GL_INVALID_OPERATION);
		codeToStr(GL_STACK_OVERFLOW);
		codeToStr(GL_STACK_UNDERFLOW);
		codeToStr(GL_OUT_OF_MEMORY);
	default:
		return "bad value";
		break;
	}

}

//=================================================================================
bool _glErrorCheck(const char* file, const int line)
{
	GLenum status;
	while ((status = glGetError()) != GL_NO_ERROR) {
		std::cout << "[" << file << ":" << line << "] Error (0x"
			<< std::hex << std::setfill('0') << std::setw(4)
			<< status << ":" << glErrorCodeToString(status) << "): "
			//<< glewGetErrorString(status)
			<< std::dec
			<< std::endl;
#if _DEBUG
		__debugbreak();
#endif
		return true;
	}
	return false;
}


namespace GLEngine {
namespace GLRenderer {
namespace Windows {

std::shared_ptr<Shaders::C_ShaderProgram> program;

//=================================================================================
C_ExplerimentWindow::C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo)
	: C_GLFWoGLWindow(wndInfo)
{
	glfwMakeContextCurrent(m_Window);
	program = Shaders::C_ShaderManager::Instance().GetProgram("basic-planes");

	m_OrbitalCamera = std::make_shared<Cameras::C_OrbitalCamera>();
	m_OrbitalCamera->setupCameraProjection(0.1f, 2 * (10.0f), static_cast<float>(wndInfo.m_width)/ static_cast<float>(wndInfo.m_height), 90.0f);
	m_OrbitalCamera->setupCameraView(10.0f, glm::vec3(0.0f),90,0);
	m_OrbitalCamera->update();

	m_FrameConstUBO = Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Buffers::UBO::C_FrameConstantsBuffer>("frameConst");

	{
		glEnable(GL_DEPTH_TEST);
		using namespace Commands;
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLClearColor>(glm::vec3(1.0f, 1.0f, 1.0f))
			)
		);
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLCullFace>(C_GLCullFace::E_FaceMode::FrontAndBack)
			)
		);
	}


	SetupWorld();
}

//=================================================================================
void C_ExplerimentWindow::Update()
{
	Shaders::C_ShaderManager::Instance().Update();

	glfwMakeContextCurrent(m_Window);
	{
		using namespace Commands;
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLClear>(C_GLClear::E_ClearBits::Color | C_GLClear::E_ClearBits::Depth)
			)
		);
	}

	auto entitiesInView = m_World.GetEntities(m_OrbitalCamera->GetFrustum());
	for (auto& entity : entitiesInView)
	{
		if (auto& graphicComponent = entity->GetComponent(Entity::E_ComponentType::Graphical)) {
			auto renderable = std::static_pointer_cast<Renderer::I_RenderableComponent>(graphicComponent);
			renderable->PerformDraw();
		}
	}



	Shaders::C_ShaderManager::Instance().ActivateShader(program);

	program->SetUniform("modelMatrix", glm::mat4(1.0f));
	program->SetUniform("modelColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	m_OrbitalCamera->adjustOrientation(0.1f, 0.1f);
	m_OrbitalCamera->update();

	glViewport(0, 0, GetWidth(), GetHeight());

	m_FrameConstUBO->SetViewProjection(m_OrbitalCamera->GetViewProjectionMatrix());
	m_FrameConstUBO->SetCameraPosition(glm::vec4(m_OrbitalCamera->GetPosition(), 1.0f));
	m_FrameConstUBO->UploadData();
	m_FrameConstUBO->Activate(true);

	m_renderer->Commit();

	m_renderer->ClearCommandBuffers();

	glBindVertexArray(0);
	Shaders::C_ShaderManager::Instance().DeactivateShader();
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

//=================================================================================
void C_ExplerimentWindow::SetupWorld()
{
	auto plane = std::make_shared<Entity::C_BasicEntity>("plane");
	m_World.AddEntity(plane);
	plane->AddComponent(std::make_shared<Components::C_StaticMesh>("scene.obj"));
}

}
}
}