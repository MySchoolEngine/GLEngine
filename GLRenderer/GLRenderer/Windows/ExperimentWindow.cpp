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
#include <Entity/IComponent.h>

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
	, m_texture("dummyTexture")
{
	glfwMakeContextCurrent(m_Window);
	program = Shaders::C_ShaderManager::Instance().GetProgram("basic-planes");

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


	SetupWorld(wndInfo);
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

	auto playerPtr = m_Player.lock();
	if (!playerPtr) {
		CORE_LOG(E_Level::Error, E_Context::Render, "Player expiretd!");
		return;
	}

	auto cameraComponent = playerPtr->GetComponent<Entity::E_ComponentType::Camera>();
	if (!cameraComponent) {
		return;
	}


	auto entitiesInView = m_World.GetEntities(cameraComponent->GetFrustum());
	for (auto& entity : entitiesInView)
	{
		if (auto renderable = entity->GetComponent<Entity::E_ComponentType::Graphical>()) {
			renderable->PerformDraw();
		}
	}


	glActiveTexture(GL_TEXTURE1);
	m_texture.bind();

	Shaders::C_ShaderManager::Instance().ActivateShader(program);

	program->SetUniform("tex", 1);
	program->SetUniform("modelMatrix", glm::mat4(1.0f));
	program->SetUniform("modelColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	std::static_pointer_cast<Cameras::C_OrbitalCamera>(cameraComponent)->adjustOrientation(0.1f, 0.1f);
	std::static_pointer_cast<Cameras::C_OrbitalCamera>(cameraComponent)->update();

	glViewport(0, 0, GetWidth(), GetHeight());

	m_FrameConstUBO->SetViewProjection(cameraComponent->GetViewProjectionMatrix());
	m_FrameConstUBO->SetCameraPosition(glm::vec4(cameraComponent->GetPosition(), 1.0f));
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
void C_ExplerimentWindow::SetupWorld(const Core::S_WindowInfo& wndInfo)
{
	{
		auto plane = std::make_shared<Entity::C_BasicEntity>("plane");
		m_World.AddEntity(plane);
		plane->AddComponent(std::make_shared<Components::C_StaticMesh>("scene.obj"));
	}
	{
		auto player = std::make_shared<Entity::C_BasicEntity>("player");
		m_Player = player;
		m_World.AddEntity(player);
		auto playerCamera = std::make_shared<Cameras::C_OrbitalCamera>();
		playerCamera->setupCameraProjection(0.1f, 2 * (10.0f), static_cast<float>(wndInfo.m_width) / static_cast<float>(wndInfo.m_height), 90.0f);
		playerCamera->setupCameraView(10.0f, glm::vec3(0.0f), 90, 0);
		playerCamera->update();
		player->AddComponent(playerCamera);
	}


	m_texture.StartGroupOp();
	m_texture.SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	m_texture.SetWrap(GL_REPEAT, GL_REPEAT);
	GLubyte data[] = {
		0, 0, 0, 255,
		255, 0, 0, 255,
		0, 0, 0, 255,
		255, 0, 0, 255,
	};
	glTexImage2D(m_texture.GetTarget(), 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	m_texture.SetDimensions({ 2,2 });
	m_texture.EndGroupOp();
}

}
}
}