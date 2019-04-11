#include <GLRendererStdafx.h>

#include <GLRenderer/Windows/ExperimentWindow.h>

#include <GLRenderer/Cameras/OrbitalCamera.h>

#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GlClearColor.h>
#include <GLRenderer/Commands/GLCullFace.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <GLRenderer/MeshLoading/Scene.h>

#include <GLRenderer/Textures/TextureLoader.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>

#include <GLRenderer/Components/StaticMesh.h>
#include <Entity/IComponent.h>

#include <Entity/BasicEntity.h>

#include <Core/EventSystem/EventDispatcher.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>

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
std::shared_ptr<Shaders::C_ShaderProgram> terrainProgram;

//=================================================================================
C_ExplerimentWindow::C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo)
	: C_GLFWoGLWindow(wndInfo)
	, m_texture("dummyTexture")
	, m_LayerStack(std::string("ExperimentalWindowLayerStack"))
{
	glfwMakeContextCurrent(m_Window);
	program = Shaders::C_ShaderManager::Instance().GetProgram("basic");
	terrainProgram = Shaders::C_ShaderManager::Instance().GetProgram("terrain");

	m_FrameConstUBO = Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Buffers::UBO::C_FrameConstantsBuffer>("frameConst");

	{
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		using namespace Commands;
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLClearColor>(glm::vec3(1.0f, 1.0f, 1.0f))
			)
		);
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLCullFace>(C_GLCullFace::E_FaceMode::Front)
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

	auto cameraComponent = GetCameraComponent();
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

	std::static_pointer_cast<Cameras::C_OrbitalCamera>(cameraComponent)->update();

	glViewport(0, 0, GetWidth(), GetHeight());

	m_FrameConstUBO->SetViewProjection(cameraComponent->GetViewProjectionMatrix());
	m_FrameConstUBO->SetCameraPosition(glm::vec4(cameraComponent->GetPosition(), 1.0f));
	m_FrameConstUBO->UploadData();
	m_FrameConstUBO->Activate(true);
	// ----- Frame init -------
	auto& shmgr = Shaders::C_ShaderManager::Instance();


	// ----- rest of scene -------
	shmgr.ActivateShader(shmgr.GetProgram("basic"));
	glActiveTexture(GL_TEXTURE0);
	m_texture.bind();

	program->SetUniform("tex", 0);
	program->SetUniform("modelMatrix", glm::mat4(1.0f));
	//program->SetUniform("modelColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	m_renderer->Commit();

	m_renderer->ClearCommandBuffers();




	// ----- terrain -------
	shmgr.ActivateShader(shmgr.GetProgram("terrain"));
	terrainProgram->SetUniform("width", (int)m_Terrain->GetWidth());
	//terrainProgram->SetUniform("height", (int)m_Terrain->GetHeight());
	terrainProgram->SetUniform("modelColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//terrainProgram->SetUniform("screen_size", glm::vec2(640, 480));


	m_Terrain->BindVAO();
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFF);
	//glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(m_Terrain->GetNumTriangles()), GL_UNSIGNED_INT, nullptr);
	m_Terrain->UnbindVAO();


	Shaders::C_ShaderManager::Instance().DeactivateShader();
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

//=================================================================================
void C_ExplerimentWindow::OnEvent(Core::I_Event& event)
{
	// base can make filtering
	T_Base::OnEvent(event);

	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_KeyPressedEvent>(std::bind(&C_ExplerimentWindow::OnKeyPressed, this, std::placeholders::_1));
}

//=================================================================================
bool C_ExplerimentWindow::OnKeyPressed(Core::C_KeyPressedEvent& event)
{
	CORE_LOG(E_Level::Info, E_Context::Render, "Heya presssed button {}", event.GetName());

	if (event.GetWindowGUID() != GetGUID()) {
		return false;
	}
	auto cameraComponent = GetCameraComponent();

	if (!cameraComponent) {
		return false;
	}

	auto camera = std::static_pointer_cast<Cameras::C_OrbitalCamera>(cameraComponent);

	if (event.GetKeyCode() == GLFW_KEY_DOWN) {
		camera->adjustOrientation(0.0f, -0.1f);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_UP) {
		camera->adjustOrientation(0.0f, 0.1f);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_LEFT) {
		camera->adjustOrientation(0.1f, 0.0f);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_RIGHT) {
		camera->adjustOrientation(-0.1f, 0.0f);
		return true;
	}


	return false;
}

//=================================================================================
void C_ExplerimentWindow::SetupWorld(const Core::S_WindowInfo& wndInfo)
{
	if (false) {
		auto plane = std::make_shared<Entity::C_BasicEntity>("plane");
		m_World.AddEntity(plane);
		plane->AddComponent(std::make_shared<Components::C_StaticMesh>("scene.obj"));
	}
	{
		auto player = std::make_shared<Entity::C_BasicEntity>("player");
		m_Player = player;
		m_World.AddEntity(player);
		float zoom = 5.0f;
		auto playerCamera = std::make_shared<Cameras::C_OrbitalCamera>();
		playerCamera->setupCameraProjection(0.1f, 2 * zoom, static_cast<float>(wndInfo.m_width) / static_cast<float>(wndInfo.m_height), 90.0f);
		playerCamera->setupCameraView(zoom, glm::vec3(0.0f), 90, 0);
		playerCamera->adjustOrientation(20.f, 20.f);
		playerCamera->update();
		player->AddComponent(playerCamera);
	}
	if (false) {
		// billboard
		Mesh::Mesh billboardMesh;
		billboardMesh.vertices.emplace_back(0, 1, 0, 1); // 1
		billboardMesh.vertices.emplace_back(0, 0, 0, 1); // 2
		billboardMesh.vertices.emplace_back(1, 1, 0, 1); // 3
		billboardMesh.vertices.emplace_back(0, 0, 0, 1); // 4 = 2
		billboardMesh.vertices.emplace_back(1, 0, 0, 1); // 5
		billboardMesh.vertices.emplace_back(1, 1, 0, 1); // 6 = 3

		billboardMesh.normals.emplace_back(0, 0, -1);
		billboardMesh.normals.emplace_back(0, 0, -1);
		billboardMesh.normals.emplace_back(0, 0, -1);
		billboardMesh.normals.emplace_back(0, 0, -1);
		billboardMesh.normals.emplace_back(0, 0, -1);
		billboardMesh.normals.emplace_back(0, 0, -1);

		billboardMesh.texcoords.emplace_back(0, 1);
		billboardMesh.texcoords.emplace_back(0, 0);
		billboardMesh.texcoords.emplace_back(1, 1);
		billboardMesh.texcoords.emplace_back(0, 0);
		billboardMesh.texcoords.emplace_back(1, 0);
		billboardMesh.texcoords.emplace_back(1, 1);

		auto plane = std::make_shared<Entity::C_BasicEntity>("plane");
		m_World.AddEntity(plane);
		plane->AddComponent(std::make_shared<Components::C_StaticMesh>(billboardMesh));
	}
	if(false){
		Textures::TextureLoader tl;
		Mesh::Texture t;
		bool retval = tl.loadTexture("Models/IMG_20151115_104149.jpg", t);

		if (!retval)
			CORE_LOG(E_Level::Error, E_Context::Render, "TExture cannot be loaded");

		m_texture.StartGroupOp();
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			(GLsizei)t.width,
			(GLsizei)t.height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			t.data.get());
		m_texture.SetDimensions({ t.width, t.height });
		m_texture.SetWrap(GL_REPEAT, GL_REPEAT);
		m_texture.SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		glGenerateMipmap(m_texture.GetTarget());
		//	ErrorCheck();

		m_texture.EndGroupOp();
	}


	m_texture.StartGroupOp();
	m_texture.SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	m_texture.SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	GLubyte data[] = {
		0, 255, 0, 255,
		255, 0, 0, 255,
		255, 0, 0, 255,
		0, 255, 0, 255,
	};
	glTexImage2D(m_texture.GetTarget(), 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	m_texture.SetDimensions({ 2,2 });
	glGenerateMipmap(m_texture.GetTarget());
	m_texture.EndGroupOp();


	m_Terrain = std::make_shared<Mesh::C_TerrainMeshResource>();
}

//=================================================================================
std::shared_ptr<Renderer::I_CameraComponent> C_ExplerimentWindow::GetCameraComponent() const
{
	auto playerPtr = m_Player.lock();
	if (!playerPtr) {
		CORE_LOG(E_Level::Error, E_Context::Render, "Player expiretd!");
		return nullptr;
	}

	auto cameraComponent = playerPtr->GetComponent<Entity::E_ComponentType::Camera>();
	if (!cameraComponent) {
		return nullptr;
	}
	return cameraComponent;
}

}
}
}