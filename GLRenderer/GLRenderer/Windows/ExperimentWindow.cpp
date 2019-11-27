#include <GLRendererStdafx.h>

#include <GLRenderer/Windows/ExperimentWindow.h>

#include <GLRenderer/Cameras/OrbitalCamera.h>

#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLEnable.h>
#include <GLRenderer/Commands/GlClearColor.h>
#include <GLRenderer/Commands/GLCullFace.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>

#include <GLRenderer/Components/TerrainMesh.h>
#include <GLRenderer/Components/StaticMesh.h>

#include <GLRenderer/Entities/TerrainEntity.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <GLRenderer/ImGui/ImGuiLayer.h>

#include <GLRenderer/MeshLoading/Scene.h>

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

#include <GLRenderer/Textures/TextureLoader.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>

#include <GLRenderer/Components/StaticMesh.h>
#include <GLRenderer/Components/SkyBox.h>
#include <GLRenderer/PersistentDebug.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/Debug.h>

#include <GLRenderer/GUI/Components/GLEntityDebugComponent.h>

#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Intersection.h>

#include <Entity/IComponent.h>
#include <Entity/BasicEntity.h>

#include <Core/EventSystem/EventDispatcher.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/AppEvent.h>

#include <imgui.h>

namespace GLEngine {
namespace GLRenderer {
namespace Windows {

//=================================================================================
C_ExplerimentWindow::C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo)
	: C_GLFWoGLWindow(wndInfo)
	, m_texture("dummyTexture")
	, m_LayerStack(std::string("ExperimentalWindowLayerStack"))
	, m_Samples("Frame Times")
	, m_GammaSlider(2.2f, 1.f,5.f, "Gamma")
	, m_ExposureSlider(1.f, .1f, 10.f, "Exposure")
	, m_VSync(false)
	, m_Spawning(false)
	, m_SpawningName("")
	, m_SpawningFilename("")
	, m_HDRFBO("HDR")
	, m_GUITexts({{
		("Avg frame time {:.2f}"),
		("Avg fps {:.2f}"),
		("Min/max frametime {:.2f}/{:.2f}")
		}})
{
	glfwMakeContextCurrent(m_Window);

	m_FrameConstUBO = Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Buffers::UBO::C_FrameConstantsBuffer>("frameConst");

	m_FrameTimer.reset();

	m_ImGUI = new ImGui::C_ImGuiLayer(m_ID);
	m_ImGUI->OnAttach(); // manual call for now.
	m_LayerStack.PushLayer(m_ImGUI);
	m_LayerStack.PushLayer(&m_CamManager);

	m_VSync.SetName("Lock FPS");
}

//=================================================================================
void C_ExplerimentWindow::Update()
{
	Shaders::C_ShaderManager::Instance().Update();
	m_ImGUI->FrameBegin();
	m_ImGUI->OnUpdate();
	//MouseSelect();

	const auto avgMsPerFrame = m_Samples.Avg();
	m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgFrametime)].UpdateText(m_Samples.Avg());
	m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgFps)].UpdateText(1000.f/ avgMsPerFrame);
	m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::MinMaxFrametime)]
		.UpdateText(
			*std::min_element(m_Samples.cbegin(), m_Samples.cend()), 
			*std::max_element(m_Samples.cbegin(), m_Samples.cend()));
	
	glfwSwapInterval(m_VSync?1:0);

	m_World.OnUpdate();

	glfwMakeContextCurrent(m_Window);
	m_HDRFBO.Bind<E_FramebufferTarget::Draw>();
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


	bool my_tool_active = true;
	::ImGui::Begin("Entities", &my_tool_active);
		if (::ImGui::Button("Spawn new terrain")) {
			m_Spawning = true;
			m_SpawningName[0] = '\0';
			m_SpawningFilename[0] = '\0';
		}
		for (const auto entity : entitiesInView) {
			bool selected = false;
			::ImGui::Selectable(entity->GetName().c_str(), &selected);
			if (selected) {
				entity->OnEvent(Core::C_UserEvent("selected"));
			}
		}
	::ImGui::End();


	if (m_Spawning) {
		::ImGui::Begin("Spawn terrain", &m_Spawning);
		::ImGui::InputText("Terrain name", m_SpawningName, 255);
		::ImGui::InputText("Terrain filename", m_SpawningFilename, 255);

		if (::ImGui::Button("Spawn")) {
			m_Spawning = false;
			auto Terrain = std::make_shared<C_TerrainEntity>(m_SpawningName);
			m_World.AddEntity(Terrain); Textures::TextureLoader tl;
			Mesh::Texture t;
			bool retval = tl.loadTexture(m_SpawningFilename, t);
			if (retval) {
				Textures::C_Texture ct(m_SpawningFilename);

				ct.StartGroupOp();
				glTexImage2D(ct.GetTarget(),
					0,
					GL_RGB,
					(GLsizei)1024,
					(GLsizei)1024,
					0,
					GL_RGBA,
					T_TypeToGL<typename std::remove_pointer<decltype(t.data)::element_type>::type>::value,
					t.data.get());
				ct.SetDimensions({ 1024, 1024 });
				ct.SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
				ct.SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
				ct.GenerateMipMaps();

				ct.EndGroupOp();

				auto patch = std::make_shared<Components::C_TerrainMesh>(std::move(ct));
				Terrain->AddPatch(patch);
				patch->SetCoord({ 0,0 });

			}
			else {
				CORE_LOG(E_Level::Error, E_Context::Entity, "Given texture {} doesn't exists", m_SpawningFilename);
				Terrain->AddPatch(glm::ivec2(0, 0));
			}
		}

		::ImGui::End();
	}


	// ----- Frame init -------
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	auto basicProgram = shmgr.GetProgram("basic");
	shmgr.ActivateShader(basicProgram);

	m_FrameConstUBO->SetView(cameraComponent->GetViewMatrix());
	m_FrameConstUBO->SetProjection(cameraComponent->GetProjectionMatrix());
	m_FrameConstUBO->SetCameraPosition(glm::vec4(cameraComponent->GetPosition(), 1.0f));

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

	// ----- Frame init -------
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					{
						RenderDoc::C_DebugScope s("CameraDebugDraw");
						std::static_pointer_cast<Cameras::C_OrbitalCamera>(cameraComponent)->DebugDraw();
					}

					shmgr.DeactivateShader();
					{
						RenderDoc::C_DebugScope s("Persistent debug");
						C_PersistentDebug::Instance().DrawAll();
					}
					{
						RenderDoc::C_DebugScope s("Merged debug");
						C_DebugDraw::Instance().DrawMergedGeoms();
					}
				}
			)
		)
	);

	m_HDRFBO.Unbind<E_FramebufferTarget::Draw>();

	{
		using namespace Commands;
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLClear>(C_GLClear::E_ClearBits::Color | C_GLClear::E_ClearBits::Depth)
			)
		);
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLViewport>(0, 0, GetWidth(), GetHeight())
			)
		);
	}

	auto HDRTexture = m_HDRFBO.GetAttachement(GL_COLOR_ATTACHMENT0);

	auto& tm = Textures::C_TextureUnitManger::Instance();
	tm.BindTextureToUnit(*(HDRTexture.get()), 0);

	m_HDRFBO.Bind<E_FramebufferTarget::Read>();

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					auto shader = shmgr.GetProgram("screenQuad");
					shmgr.ActivateShader(shader);
					shader->SetUniform("gamma", m_GammaSlider.GetValue());
					shader->SetUniform("exposure", m_ExposureSlider.GetValue());
					shader->BindSampler(*(HDRTexture.get()), 0);
				}
			)
		)
	);

	m_ScreenQuad->PerformDraw();
	
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					shmgr.DeactivateShader();
				}
			)
		)
	);


	m_HDRFBO.Unbind<E_FramebufferTarget::Read>();

	// ----- Actual rendering --

	{
		RenderDoc::C_DebugScope s("RendererCommit");
		m_renderer->Commit();
	}
	m_renderer->ClearCommandBuffers();
	// ----- Actual rendering --

	{
		RenderDoc::C_DebugScope s("ImGUI");
		static_cast<C_OGLRenderer*>(m_renderer.get())->DrawControls();
		m_ImGUI->FrameEnd();
	}


	glfwSwapBuffers(m_Window);
	sampleTime(float(m_FrameTimer.getElapsedTimeFromLastQueryMilliseconds()));	
}

//=================================================================================
void C_ExplerimentWindow::sampleTime(double new_sample) 
{
	m_Samples.Sample(static_cast<float>(new_sample));
}

//=================================================================================
void C_ExplerimentWindow::OnEvent(Core::I_Event& event)
{
	RenderDoc::C_DebugScope s("OnEvent-Fail");
	// base can make filtering
	T_Base::OnEvent(event);

	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_KeyPressedEvent>(std::bind(&C_ExplerimentWindow::OnKeyPressed, this, std::placeholders::_1));
	d.Dispatch<Core::C_AppEvent>(std::bind(&C_ExplerimentWindow::OnAppInit, this, std::placeholders::_1));
	d.Dispatch<Core::C_WindowResizedEvent>(std::bind(&C_ExplerimentWindow::OnWindowResized, this, std::placeholders::_1));

	m_LayerStack.OnEvent(event);
}

//=================================================================================
bool C_ExplerimentWindow::OnKeyPressed(Core::C_KeyPressedEvent& event)
{
	// if (event.GetWindowGUID() != GetGUID()) {
	// 	return false;
	// }


	return false;
}

//=================================================================================
bool C_ExplerimentWindow::OnAppInit(Core::C_AppEvent& event)
{
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

	SetupWorld();

	auto HDRTexture = std::make_shared<Textures::C_Texture>("hdrTexture");

	HDRTexture->bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GetWidth(), GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
	HDRTexture->SetFilter(GL_LINEAR, GL_LINEAR);
	m_HDRFBO.AttachTexture(GL_COLOR_ATTACHMENT0, HDRTexture);
	HDRTexture->unbind();

	auto depthStencilTexture = std::make_shared<Textures::C_Texture>("hdrDepthTexture");

	depthStencilTexture->bind();
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, GetWidth(), GetHeight(), 0,
		GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr
	);
	depthStencilTexture->SetFilter(GL_LINEAR, GL_LINEAR);
	m_HDRFBO.AttachTexture(GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture);
	depthStencilTexture->unbind();


	return false;
}

//=================================================================================
bool C_ExplerimentWindow::OnWindowResized(Core::C_WindowResizedEvent& event)
{
	auto HDRTexture = m_HDRFBO.GetAttachement(GL_COLOR_ATTACHMENT0);
	HDRTexture->bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, event.GetWidth(), event.GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
	HDRTexture->SetFilter(GL_LINEAR, GL_LINEAR);
	HDRTexture->unbind();

	auto depthStencilTexture = m_HDRFBO.GetAttachement(GL_DEPTH_STENCIL_ATTACHMENT);

	depthStencilTexture->bind();
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, event.GetWidth(), event.GetHeight(), 0,
		GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr
	);
	depthStencilTexture->SetFilter(GL_LINEAR, GL_LINEAR);
	depthStencilTexture->unbind();

	return true;
}

//=================================================================================
void C_ExplerimentWindow::Init(const Core::S_WindowInfo& wndInfo)
{
	GLFW::C_GLFWoGLWindow::Init(wndInfo);
}

//=================================================================================
void C_ExplerimentWindow::SetupWorld()
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
		playerCamera->setupCameraProjection(0.1f, 2 * zoom*100, static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()), 90.0f);
		playerCamera->setupCameraView(zoom, glm::vec3(0.0f), 90, 0);
		playerCamera->adjustOrientation(20.f, 20.f);
		playerCamera->Update();
		player->AddComponent(playerCamera);
		player->AddComponent(std::make_shared<GUI::C_GLEntityDebugComponent>(player));
		m_CamManager.ActivateCamera(playerCamera);
	}
	{
		// billboard
		Mesh::Mesh billboardMesh;
		billboardMesh.vertices.emplace_back(-1.f,  1.f, 0, 1); // 1
		billboardMesh.vertices.emplace_back(-1.f, -1.f, 0, 1); // 2
		billboardMesh.vertices.emplace_back( 1.0f, 1.0f, 0, 1); // 3
		billboardMesh.vertices.emplace_back(-1.f, -1.f, 0, 1); // 4 = 2
		billboardMesh.vertices.emplace_back( 1.f, -1.f, 0, 1); // 5
		billboardMesh.vertices.emplace_back( 1.0f, 1.0f, 0, 1); // 6 = 3


		billboardMesh.texcoords.emplace_back(0, 1);
		billboardMesh.texcoords.emplace_back(0, 0);
		billboardMesh.texcoords.emplace_back(1, 1);
		billboardMesh.texcoords.emplace_back(0, 0);
		billboardMesh.texcoords.emplace_back(1, 0);
		billboardMesh.texcoords.emplace_back(1, 1);

		m_ScreenQuad = std::make_shared<Components::C_StaticMesh>(billboardMesh);
	}
	if(true){
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
		m_texture.SetTexData2D(0, t);
		m_texture.SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
		m_texture.SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		m_texture.GenerateMipMaps();

		m_texture.EndGroupOp();
	}
	if(false)
	{
		auto Terrain = std::make_shared<C_TerrainEntity>();
		m_World.AddEntity(Terrain);
		Terrain->AddPatch(glm::ivec2(0, 0));
		Terrain->AddPatch(glm::ivec2(0, -1));
		Terrain->AddPatch(glm::ivec2(-1, 0));
		Terrain->AddPatch(glm::ivec2(-1, -1));
	}


	m_texture.StartGroupOp();
	m_texture.SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	m_texture.SetWrap(E_WrapFunction::ClampToEdge, E_WrapFunction::ClampToEdge);
	GLubyte data[] = {
		0, 255, 0, 255,
		255, 0, 0, 255,
		255, 0, 0, 255,
		0, 255, 0, 255,
	};
	glTexImage2D(m_texture.GetTarget(), 0, GL_RGBA, 2, 2, 0, GL_RGBA, T_TypeToGL<GLubyte>::value, data);
	m_texture.SetDimensions({ 2,2 });
	m_texture.GenerateMipMaps();
	m_texture.EndGroupOp();




	auto& guiMGR = m_ImGUI->GetGUIMgr();

	m_FrameStatsGUID = guiMGR.CreateGUIWindow("Frame stats");
	auto* frameStats = guiMGR.GetWindow(m_FrameStatsGUID);

	frameStats->AddComponent(m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgFrametime)]);
	frameStats->AddComponent(m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgFps)]);
	frameStats->AddComponent(m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::MinMaxFrametime)]);
	frameStats->AddComponent(m_Samples);
	frameStats->AddComponent(m_VSync);
	frameStats->SetVisible(true);

	m_HDRSettingsGUID = guiMGR.CreateGUIWindow("HDR Settings");
	auto* hdrSettings = guiMGR.GetWindow(m_HDRSettingsGUID);

	hdrSettings->AddComponent(m_GammaSlider);
	hdrSettings->AddComponent(m_ExposureSlider);
	hdrSettings->SetVisible(true);
}

//=================================================================================
void C_ExplerimentWindow::MouseSelect()
{
	if (m_ImGUI->CapturingMouse()) {
		return;
	}

	auto screenCoord = GetInput().GetMousePosition();
	auto camera = GetCameraComponent();

	float x = (2.0f * screenCoord.first) / GetWidth() - 1.0f;
	float y = 1.0f - (2.0f * screenCoord.second) / GetHeight();
	float z = 1.0f;
	glm::vec3 ray_nds = glm::vec3(x, y, z);
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	glm::vec4 ray_eye = glm::inverse(camera->GetProjectionMatrix()) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	glm::vec4 ray_wor = glm::inverse(camera->GetViewMatrix()) * ray_eye;
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);
	glm::vec4 cameraOrigin = glm::vec4(camera->GetPosition(), 1.0f);

	C_PersistentDebug::Instance().DrawLine(cameraOrigin, cameraOrigin + ray_wor, glm::vec3(0, 1, 0));
	{
		using namespace Physics::Primitives;
		S_Ray ray;
		ray.origin = cameraOrigin;
		ray.direction = ray_wor;
		S_RayIntersection inter = m_World.Select(ray);
		if (inter.distance > 0) {
			auto entity = m_World.GetEntity(inter.entityId);
			if (entity) {
				entity->OnEvent(Core::C_UserEvent("selected"));
			}
			//std::static_pointer_cast<Cameras::C_OrbitalCamera>(camera)->setCenterPoint(inter.intersectionPoint);
			//std::static_pointer_cast<Cameras::C_OrbitalCamera>(camera)->update();
		}
	}
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