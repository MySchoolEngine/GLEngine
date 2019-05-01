#include <GLRendererStdafx.h>

#include <GLRenderer/Windows/ExperimentWindow.h>

#include <GLRenderer/Cameras/OrbitalCamera.h>

#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLEnable.h>
#include <GLRenderer/Commands/GlClearColor.h>
#include <GLRenderer/Commands/GLCullFace.h>
#include <GLRenderer/Commands/GLViewport.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <GLRenderer/ImGui/ImGuiLayer.h>

#include <GLRenderer/MeshLoading/Scene.h>

#include <GLRenderer/Textures/TextureLoader.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>

#include <GLRenderer/Components/StaticMesh.h>
#include <GLRenderer/Components/SkyBox.h>
#include <GLRenderer/PersistentDebug.h>

#include <Entity/IComponent.h>
#include <Entity/BasicEntity.h>

#include <Core/EventSystem/EventDispatcher.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>

#include <imgui.h>


namespace GLEngine {
namespace GLRenderer {
namespace Windows {

//=================================================================================
C_ExplerimentWindow::C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo)
	: C_GLFWoGLWindow(wndInfo)
	, m_texture("dummyTexture")
	, m_LayerStack(std::string("ExperimentalWindowLayerStack"))
{
	glfwMakeContextCurrent(m_Window);

	m_FrameConstUBO = Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Buffers::UBO::C_FrameConstantsBuffer>("frameConst");

	{
		using namespace Commands;
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLEnable>(C_GLEnable::E_GLEnableValues::DEPTH_TEST)
			)
		);
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLEnable>(C_GLEnable::E_GLEnableValues::CULL_FACE)
			)
		);
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
	m_ImGUI = new ImGui::C_ImGuiLayer(m_ID);
	m_ImGUI->OnAttach(); // manual call for now.
	m_LayerStack.PushLayer(m_ImGUI);
	m_LayerStack.PushLayer(&m_CamManager);
}

//=================================================================================
void C_ExplerimentWindow::Update()
{
	Shaders::C_ShaderManager::Instance().Update();
	m_ImGUI->FrameBegin();
	m_ImGUI->OnUpdate();

	::ImGui::ShowDemoWindow();
	bool my_tool_active = true;
	::ImGui::Begin("Terrain visualizations", &my_tool_active);
	WholeTerrain([](T_TerrainPtr terrain) {
		::ImGui::Image((void*)terrain->GetTexture().GetTexture() , 
		{	
			256,
			256
		},
			{ 0,1 }, { 1,0 });
	});
	::ImGui::End();

	::ImGui::Begin("Terrain controls", &my_tool_active, ImGuiWindowFlags_MenuBar);
	if (::ImGui::BeginMenuBar())
	{
		if (::ImGui::BeginMenu("File"))
		{
			if (::ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
			if (::ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
			::ImGui::EndMenu();
		}
		::ImGui::EndMenuBar();
	}
	if (::ImGui::Button("Rain"))
	{
		// do stuff
	}
	bool value = m_TerrainComp[0]->UsingPerlinNoise();
	::ImGui::Checkbox("PerlinNoise", &value);
	WholeTerrain([&](T_TerrainPtr terrain) {
		terrain->UsePerlinNoise(value);
	});
	::ImGui::End();

	m_World.OnUpdate();

	glfwMakeContextCurrent(m_Window);
	{
		using namespace Commands;
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLClear>(C_GLClear::E_ClearBits::Color | C_GLClear::E_ClearBits::Depth)
			)
		);
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLViewport>(0,0, GetWidth(), GetHeight())
			)
		);
	}

	auto cameraComponent = m_CamManager.GetActiveCamera();
	if (!cameraComponent) {
		return;
	}


	auto entitiesInView = m_World.GetEntities(cameraComponent->GetFrustum());


	my_tool_active = true;
	::ImGui::Begin("Entities", &my_tool_active);
	for (const auto entity : entitiesInView) {
		bool selected = false;
		::ImGui::Selectable(entity->GetName().c_str(), &selected);
		if (selected) {
			entity->OnEvent(Core::C_UserEvent("selected"));
		}
	}
	::ImGui::End();

	for (auto& entity : entitiesInView)
	{
		if (auto renderable = entity->GetComponent<Entity::E_ComponentType::Graphical>()) {
			renderable->PerformDraw();
		}
	}

	std::static_pointer_cast<Cameras::C_OrbitalCamera>(cameraComponent)->update();


	// ----- Frame init -------
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	auto basicProgram = shmgr.GetProgram("basic");
	shmgr.ActivateShader(basicProgram);

	m_FrameConstUBO->SetViewProjection(cameraComponent->GetViewProjectionMatrix());
	m_FrameConstUBO->SetCameraPosition(glm::vec4(cameraComponent->GetPosition(), 1.0f));
	m_FrameConstUBO->UploadData();
	m_FrameConstUBO->Activate(true);


	glActiveTexture(GL_TEXTURE0);

	m_TerrainComp[0]->GetTexture().bind();

	basicProgram->SetUniform("tex", 0);
	basicProgram->SetUniform("modelMatrix", glm::mat4(1.0f));
	// ----- Frame init -------

	// ----- Actual rendering --

	m_renderer->Commit();
	m_renderer->ClearCommandBuffers();
	// ----- Actual rendering --


	{
		RenderDoc::C_DebugScope s("CameraDebugDraw");
		std::static_pointer_cast<Cameras::C_OrbitalCamera>(cameraComponent)->DebugDraw();
	}

	m_TerrainComp[0]->GetTexture().unbind();

	shmgr.DeactivateShader();

	{
		RenderDoc::C_DebugScope s("ImGUI");
		m_ImGUI->FrameEnd();
	}
	{
		RenderDoc::C_DebugScope s("Persistent debug");
		C_PersistentDebug::Instance().DrawAll();
	}


	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

//=================================================================================
void C_ExplerimentWindow::OnEvent(Core::I_Event& event)
{
	RenderDoc::C_DebugScope s("OnEvent-Fail");
	// base can make filtering
	T_Base::OnEvent(event);

	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_KeyPressedEvent>(std::bind(&C_ExplerimentWindow::OnKeyPressed, this, std::placeholders::_1));

	m_LayerStack.OnEvent(event);
}

//=================================================================================
bool C_ExplerimentWindow::OnKeyPressed(Core::C_KeyPressedEvent& event)
{
	CORE_LOG(E_Level::Info, E_Context::Render, "Heya presssed button {}", event.GetName());

	if (event.GetWindowGUID() != GetGUID()) {
		return false;
	}

	if (event.GetKeyCode() == GLFW_KEY_I) {
		WholeTerrain([](T_TerrainPtr terrain) {
			terrain->IncreaseFreq();
		});
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_O) {
		WholeTerrain([](T_TerrainPtr terrain) {
			terrain->DecreaseFreq();
		});
		return true;
	}

	if (event.GetKeyCode() == GLFW_KEY_K) {
		WholeTerrain([](T_TerrainPtr terrain) {
			terrain->IncreaseSQ();
		});
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_L) {
		WholeTerrain([](T_TerrainPtr terrain) {
			terrain->DecreaseSQ();
		});
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
		playerCamera->setupCameraProjection(0.1f, 2 * zoom*100, static_cast<float>(wndInfo.m_width) / static_cast<float>(wndInfo.m_height), 90.0f);
		playerCamera->setupCameraView(zoom, glm::vec3(0.0f), 90, 0);
		playerCamera->adjustOrientation(20.f, 20.f);
		playerCamera->update();
		player->AddComponent(playerCamera);
		m_CamManager.ActivateCamera(playerCamera);
	}
	if(false){
		// billboard
		Mesh::Mesh billboardMesh;
		billboardMesh.vertices.emplace_back(0, 10, 0, 1); // 1
		billboardMesh.vertices.emplace_back(0, 0, 0, 1); // 2
		billboardMesh.vertices.emplace_back(10, 10, 0, 1); // 3
		billboardMesh.vertices.emplace_back(0, 0, 0, 1); // 4 = 2
		billboardMesh.vertices.emplace_back(10, 0, 0, 1); // 5
		billboardMesh.vertices.emplace_back(10, 10, 0, 1); // 6 = 3

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
		using Components::C_SkyBox;
		auto skyboxComp = std::make_shared<C_SkyBox>();
		skyboxComp->AddTexture(C_SkyBox::E_Side::Top, std::string("SkyBoxes/mp_crimelem/criminal-element_up.tga"));
		skyboxComp->AddTexture(C_SkyBox::E_Side::Bottom, std::string("SkyBoxes/mp_crimelem/criminal-element_dn.tga"));
		skyboxComp->AddTexture(C_SkyBox::E_Side::Right, std::string("SkyBoxes/mp_crimelem/criminal-element_rt.tga"));
		skyboxComp->AddTexture(C_SkyBox::E_Side::Left, std::string("SkyBoxes/mp_crimelem/criminal-element_lf.tga"));
		skyboxComp->AddTexture(C_SkyBox::E_Side::Forward, std::string("SkyBoxes/mp_crimelem/criminal-element_ft.tga"));
		skyboxComp->AddTexture(C_SkyBox::E_Side::Back, std::string("SkyBoxes/mp_crimelem/criminal-element_bk.tga"));

		auto skybox = std::make_shared<Entity::C_BasicEntity>("SkyBox");
		m_World.AddEntity(skybox);
		skybox->AddComponent(skyboxComp);
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
		m_texture.GenerateMipMaps();

		m_texture.EndGroupOp();
	}

	{
		auto comp = std::make_shared<Components::C_TerrainMesh>();

		auto terrain = std::make_shared<Entity::C_BasicEntity>("terrain1");
		m_World.AddEntity(terrain);
		terrain->AddComponent(comp);
		m_TerrainComp.push_back(comp);
	}
	{
		auto comp = std::make_shared<Components::C_TerrainMesh>();

		auto terrain = std::make_shared<Entity::C_BasicEntity>("terrain2");
		m_World.AddEntity(terrain);
		terrain->AddComponent(comp);
		comp->SetCoord(glm::ivec2(0, 1));
		m_TerrainComp.push_back(comp);
	}
	{
		auto comp = std::make_shared<Components::C_TerrainMesh>();

		auto terrain = std::make_shared<Entity::C_BasicEntity>("terrain3");
		m_World.AddEntity(terrain);
		terrain->AddComponent(comp);
		comp->SetCoord(glm::ivec2(1, 1));
		m_TerrainComp.push_back(comp);
	}
	{
		auto comp = std::make_shared<Components::C_TerrainMesh>();

		auto terrain = std::make_shared<Entity::C_BasicEntity>("terrain4");
		m_World.AddEntity(terrain);
		terrain->AddComponent(comp);
		comp->SetCoord(glm::ivec2(1, 0));
		m_TerrainComp.push_back(comp);
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
}

//=================================================================================
void C_ExplerimentWindow::WholeTerrain(std::function<void(T_TerrainPtr)> lambda)
{
	std::for_each(m_TerrainComp.begin(), m_TerrainComp.end(), lambda);
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