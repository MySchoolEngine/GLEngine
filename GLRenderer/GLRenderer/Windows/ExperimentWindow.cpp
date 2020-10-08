#include <GLRendererStdafx.h>

#include <GLRenderer/Windows/ExperimentWindow.h>

#include <GLRenderer/Commands/GLEnable.h>
#include <GLRenderer/Commands/GlClearColor.h>
#include <GLRenderer/Commands/GLCullFace.h>
#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>

#include <GLRenderer/Components/ComponentBuilderFactory.h>

#include <GLRenderer/Materials/MaterialBuffer.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <GLRenderer/ImGui/GLImGuiLayer.h>

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

#include <GLRenderer/Textures/TextureLoader.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

#include <GLRenderer/Buffers/UBO/ModelData.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>

#include <GLRenderer/PersistentDebug.h>
#include <GLRenderer/OGLRenderer.h>

#include <GLRenderer/Lights/GLAreaLight.h>

#include <GUI/ConsoleWindow.h>
#include <Entity/EntitiesWindow.h>

#include <Renderer/Mesh/Scene.h>
#include <Renderer/Cameras/OrbitalCamera.h>
#include <Renderer/Cameras/FreelookCamera.h>
#include <Renderer/Materials/MaterialManager.h>

#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Intersection.h>

#include <Entity/IComponent.h>
#include <Entity/BasicEntity.h>

#include <Entity/ComponentManager.h>

#include <Core/EventSystem/EventDispatcher.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/AppEvent.h>

#include <pugixml.hpp>

namespace GLEngine {
namespace GLRenderer {
namespace Windows {

//=================================================================================
C_ExplerimentWindow::C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo)
	: C_GLFWoGLWindow(wndInfo)
	, m_LayerStack(std::string("ExperimentalWindowLayerStack"))
	, m_Samples("Frame Times")
	, m_GammaSlider(1.2f, 1.f,5.f, "Gamma")
	, m_ExposureSlider(1.5f, .1f, 10.f, "Exposure")
	, m_VSync(false)
	, m_HDRFBO(nullptr)
	, m_World(std::make_shared<Entity::C_EntityManager>())
	, m_MainPass(nullptr)
	, m_ShadowPass(nullptr)
	, m_GUITexts({{
		("Avg frame time {:.2f}"),
		("Avg fps {:.2f}"),
		("Min/max frametime {:.2f}/{:.2f}")
		}})
	, m_Windows("Windows")
{
	glfwMakeContextCurrent(m_Window);

	m_FrameTimer.reset();

	m_ImGUI = new C_GLImGUILayer(m_ID);
	m_ImGUI->OnAttach(); // manual call for now.
	m_LayerStack.PushLayer(m_ImGUI);
	m_LayerStack.PushLayer(&m_CamManager);

	m_VSync.SetName("Lock FPS");

	Entity::C_ComponentManager::Instance();
}

//=================================================================================
C_ExplerimentWindow::~C_ExplerimentWindow() {
	static_cast<C_OGLRenderer*>(m_renderer.get())->DestroyControls(m_ImGUI->GetGUIMgr());

	auto& guiMGR = m_ImGUI->GetGUIMgr();
	guiMGR.DestroyWindow(m_EntitiesWindowGUID);
	guiMGR.DestroyWindow(m_ConsoleWindowGUID);
	guiMGR.DestroyWindow(m_FrameStatsGUID);
	guiMGR.DestroyWindow(m_HDRSettingsGUID);
};

//=================================================================================
void C_ExplerimentWindow::Update()
{
	Shaders::C_ShaderManager::Instance().Update();
	m_ImGUI->FrameBegin();
	m_ImGUI->OnUpdate();
	//MouseSelect();
	C_DebugDraw::Instance().DrawAxis(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0, 1.f, 0.0f), glm::vec3(0.f, 0.f, 1.f));

	const auto avgMsPerFrame = m_Samples.Avg();
	m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgFrametime)].UpdateText(m_Samples.Avg());
	m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgFps)].UpdateText(1000.f/ avgMsPerFrame);
	m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::MinMaxFrametime)]
		.UpdateText(
			*std::min_element(m_Samples.cbegin(), m_Samples.cend()), 
			*std::max_element(m_Samples.cbegin(), m_Samples.cend()));
	
	glfwSwapInterval(m_VSync?1:0);

	m_World->OnUpdate();

	glfwMakeContextCurrent(m_Window);

	//m_ShadowPass->Render();

	m_HDRFBO->Bind<E_FramebufferTarget::Draw>();

	const auto camera = m_CamManager.GetActiveCamera();
	GLE_ASSERT(camera, "No active camera");

	m_MainPass->Render(camera, GetWidth(), GetHeight());

	// ----- Frame init -------
	auto& shmgr = Shaders::C_ShaderManager::Instance();

	shmgr.DeactivateShader();
	// ----- Frame init -------

	{
		RenderDoc::C_DebugScope s("Persistent debug");
		C_PersistentDebug::Instance().DrawAll();
	}

	{
		RenderDoc::C_DebugScope s("Merged debug");
		C_DebugDraw::Instance().DrawMergedGeoms();
	}

	m_HDRFBO->Unbind<E_FramebufferTarget::Draw>();

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

	auto HDRTexture = m_HDRFBO->GetAttachement(GL_COLOR_ATTACHMENT0);

	auto& tm = Textures::C_TextureUnitManger::Instance();
	tm.BindTextureToUnit(*(HDRTexture.get()), 0);

	m_HDRFBO->Bind<E_FramebufferTarget::Read>();

	auto shader = shmgr.GetProgram("screenQuad");
	shmgr.ActivateShader(shader);

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[this, shader]() {
					shader->SetUniform("gamma", m_GammaSlider.GetValue());
					shader->SetUniform("exposure", m_ExposureSlider.GetValue());
					shader->SetUniform("hdrBuffer", 0);
				}, "Update HDR"
			)
		)
	);

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_ScreenQuad)
		)
	);

	shmgr.DeactivateShader();


	m_HDRFBO->Unbind<E_FramebufferTarget::Read>();

	{
		RenderDoc::C_DebugScope s("ImGUI");
		Core::C_Application::Get().GetActiveRenderer()->AddCommand(
			std::move(
				std::make_unique<Commands::HACK::C_LambdaCommand>(
					[this, shader]() {
						m_ImGUI->FrameEnd();
					}, "m_ImGUI->FrameEnd"
				)
			)
		);
	}

	// commit of final commands - from commit few lines above
	m_renderer->SortCommands();
	m_renderer->Commit();
	m_renderer->ClearCommandBuffers();
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
				std::make_unique<C_GLEnable>(C_GLEnable::E_GLEnableValues::BLEND)
			)
		);
		m_renderer->AddCommand(
			std::move(
				std::make_unique<HACK::C_LambdaCommand>([]() {
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					})
			)
		);
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLClearColor>(glm::vec3(0.0f, 0.0f, 0.0f))
			)
		);
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLCullFace>(C_GLCullFace::E_FaceMode::Front)
			)
		);
	}

	Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Material::C_MaterialsBuffer>("materials");
	Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Buffers::UBO::C_ModelData>("modelData");

	SetupWorld();

	auto HDRTexture = std::make_shared<Textures::C_Texture>("hdrTexture");

	HDRTexture->bind();
	// HDRTexture setup 
	HDRTexture->SetDimensions({ GetWidth(), GetHeight() });
	HDRTexture->SetInternalFormat(GL_RGBA16F, GL_RGBA, GL_FLOAT);
	HDRTexture->SetFilter(GL_LINEAR, GL_LINEAR);
	// ~HDRTexture setup 
	m_HDRFBO->AttachTexture(GL_COLOR_ATTACHMENT0, HDRTexture);
	HDRTexture->unbind();

	auto depthStencilTexture = std::make_shared<Textures::C_Texture>("hdrDepthTexture");

	depthStencilTexture->bind();
	// depthStencilTexture setup 
	depthStencilTexture->SetDimensions({ GetWidth(), GetHeight() });
	depthStencilTexture->SetInternalFormat(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
	depthStencilTexture->SetFilter(GL_LINEAR, GL_LINEAR);
	// ~depthStencilTexture setup 
	m_HDRFBO->AttachTexture(GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTexture);
	depthStencilTexture->unbind();


	return false;
}

//=================================================================================
bool C_ExplerimentWindow::OnWindowResized(Core::C_WindowResizedEvent& event)
{
	auto HDRTexture = m_HDRFBO->GetAttachement(GL_COLOR_ATTACHMENT0);
	HDRTexture->bind();
	HDRTexture->SetDimensions({ event.GetWidth(), event.GetHeight() });
	HDRTexture->SetInternalFormat(GL_RGBA16F, GL_RGBA, GL_FLOAT);
	HDRTexture->SetFilter(GL_LINEAR, GL_LINEAR);
	HDRTexture->unbind();

	auto depthStencilTexture = m_HDRFBO->GetAttachement(GL_DEPTH_STENCIL_ATTACHMENT);

	depthStencilTexture->bind();
	depthStencilTexture->SetDimensions({ event.GetWidth(), event.GetHeight() });
	depthStencilTexture->SetInternalFormat(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
	depthStencilTexture->SetFilter(GL_LINEAR, GL_LINEAR);
	depthStencilTexture->unbind();

	return true;
}

//=================================================================================
void C_ExplerimentWindow::SetupWorld()
{
	m_MainPass = std::make_unique<C_MainPassTechnique>(m_World);
	m_HDRFBO = std::make_unique<C_Framebuffer>("HDR");
	if (!m_World->LoadLevel("Levels/dark.xml", std::make_unique<Components::C_ComponentBuilderFactory>()))
	{
		CORE_LOG(E_Level::Warning, E_Context::Render, "Level not loaded");
		return;
	}

	{
		m_Player = m_World->GetEntity("Player");

		auto player = m_Player.lock();
		if (player)
		{
			float zoom = 5.0f;
			auto playerCamera = std::make_shared<Renderer::Cameras::C_OrbitalCamera>(player);
			playerCamera->setupCameraProjection(0.1f, 2 * zoom * 200, static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()), 90.0f);
			playerCamera->setupCameraView(zoom, glm::vec3(0.0f), 90, 0);
			playerCamera->adjustOrientation(20.f, 20.f);
			playerCamera->Update();
			player->AddComponent(playerCamera);
			m_CamManager.ActivateCamera(playerCamera);

			// area light
			auto arealight = std::make_shared<C_GLAreaLight>(player);
			player->AddComponent(arealight);
			// 
			// m_ShadowPass = std::make_shared<C_ShadowMapTechnique>(m_World, std::static_pointer_cast<Renderer::I_Light>( arealight));
		}
	}
	{
		// billboard
		Renderer::MeshData::Mesh billboardMesh;
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

		m_ScreenQuad = std::make_shared<Mesh::C_StaticMeshResource>(billboardMesh);
	}

	auto& guiMGR = m_ImGUI->GetGUIMgr();

	// Console window
	{
		m_ConsoleWindowGUID = NextGUID();

		auto consoleWindow = new GUI::C_ConsoleWindow(m_ConsoleWindowGUID);
		guiMGR.AddCustomWindow(consoleWindow);
		consoleWindow->SetVisible();
	}

	// Entity window
	{
		m_EntitiesWindowGUID = NextGUID();

		auto entitiesWindow = new Entity::C_EntitiesWindow(m_EntitiesWindowGUID, m_World);
		guiMGR.AddCustomWindow(entitiesWindow);
		entitiesWindow->SetVisible();
	}

	m_FrameStatsGUID = guiMGR.CreateGUIWindow("Frame stats");
	auto* frameStats = guiMGR.GetWindow(m_FrameStatsGUID);

	frameStats->AddComponent(m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgFrametime)]);
	frameStats->AddComponent(m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgFps)]);
	frameStats->AddComponent(m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::MinMaxFrametime)]);
	frameStats->AddComponent(m_Samples);
	frameStats->AddComponent(m_VSync);
	frameStats->SetVisible(true);
	frameStats->AddMenu(m_Windows);

	m_HDRSettingsGUID = guiMGR.CreateGUIWindow("HDR Settings");
	auto* hdrSettings = guiMGR.GetWindow(m_HDRSettingsGUID);

	hdrSettings->AddComponent(m_GammaSlider);
	hdrSettings->AddComponent(m_ExposureSlider);


	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItemOpenWindow>("HDR Settings", m_HDRSettingsGUID, guiMGR));

	const auto rendererWindow = static_cast<C_OGLRenderer*>(m_renderer.get())->SetupControls(guiMGR);
	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItemOpenWindow>("Renderer", rendererWindow, guiMGR));
}

//=================================================================================
void C_ExplerimentWindow::MouseSelect()
{
	if (m_ImGUI->CapturingMouse()) {
		return;
	}

	auto screenCoord = GetInput().GetMousePosition();
	auto camera = m_CamManager.GetActiveCamera();

	const auto clipPosition = ToClipSpace({ screenCoord.first, screenCoord.second });

	using namespace Physics::Primitives;
	const auto ray = camera->GetRay(clipPosition);;
	C_PersistentDebug::Instance().DrawLine(ray.origin, ray.origin + ray.direction, glm::vec3(0, 1, 0));
	const S_RayIntersection inter = m_World->Select(ray);
	if (inter.distance > 0) {
		auto entity = m_World->GetEntity(inter.entityId);
		if (entity) {
			entity->OnEvent(Core::C_UserEvent("selected"));
		}
	}
}

}
}
}