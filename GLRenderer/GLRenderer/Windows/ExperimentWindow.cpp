#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/ModelData.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLCullFace.h>
#include <GLRenderer/Commands/GLEnable.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/Commands/GlClearColor.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Components/SkeletalMesh.h>
#include <GLRenderer/Components/SkyBox.h>
#include <GLRenderer/Debug.h>
#include <GLRenderer/GLRenderInterface.h>
#include <GLRenderer/GLResourceManager.h>
#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>
#include <GLRenderer/ImGui/GLImGUILayer.h>
#include <GLRenderer/Materials/MaterialBuffer.h>
#include <GLRenderer/OGLDevice.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/PersistentDebug.h>
#include <GLRenderer/RenderInterface.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/SunShadowMapTechnique.h>
#include <GLRenderer/Textures/TextureManager.h>
#include <GLRenderer/Textures/TextureUnitManager.h>
#include <GLRenderer/Windows/ExperimentWindow.h>

#include <Renderer/Cameras/OrbitalCamera.h>
#include <Renderer/Components/StaticMeshHandles.h>
#include <Renderer/Lights/SunLight.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/RayCasting/Geometry/TrimeshModel.h>
#include <Renderer/Textures/TextureResource.h>
#include <Renderer/Textures/TextureView.h>
#include <Renderer/Windows/RayTrace.h>

#include <Editor/Editors/ImageEditor.h>
#include <Editor/EntityEditor/EntitiesWindow.h>
#include <Editor/EntityEditor/EntityEditor.h>

#include <GUI/ConsoleWindow.h>
#include <GUI/FileDialogWindow.h>

#include <Physics/Primitives/Intersection.h>
#include <Physics/Primitives/Ray.h>

#include <Entity/BasicEntity.h>
#include <Entity/ComponentManager.h>

#include <Core/EventSystem/Event/AppEvent.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/EventDispatcher.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/EnumUtils.h>
#include <Utils/Serialization/XMLDeserialize.h>
#include <Utils/Serialization/XMLSerialize.h>
#include <Utils/StdVectorUtils.h>

#include <glm/gtx/transform.hpp>

#include <imgui.h>

namespace GLEngine::GLRenderer::Windows {

//=================================================================================
C_ExperimentWindow::C_ExperimentWindow(const Core::S_WindowInfo& wndInfo)
	: C_GLFWoGLWindow(wndInfo)
	, m_LayerStack(std::string("ExperimentalWindowLayerStack"))
	, m_Samples("Frame Times")
	, m_GammaSlider(2.2f, 1.f, 5.f, "Gamma")
	, m_ExposureSlider(1.f, .1f, 10.f, "Exposure")
	, m_VSync(true)
	, m_HDRFBO(nullptr)
	, m_HDRFBOAtmosphere(nullptr)
	, m_World(std::make_shared<Entity::C_EntityManager>())
	, m_MainPass(nullptr)
	, m_ShadowPass(nullptr)
	, m_GUITexts({{GUI::C_FormatedText("Avg frame time {:.2f}"), GUI::C_FormatedText("Avg fps {:.2f}"), GUI::C_FormatedText("Min/max frametime {:.2f}/{:.2f}")}})
	, m_Windows(std::string("Windows"))
	, m_EditorLayer(*&C_DebugDraw::Instance(), GetInput(), {0, 0, GetSize()}) //< viewport could be different from windowsize in the future
{
	glfwMakeContextCurrent(m_Window);

	m_FrameTimer.reset();

	m_ImGUI = new C_GLImGUILayer(m_ID);
	m_ImGUI->OnAttach(); // manual call for now.
	m_LayerStack.PushLayer(m_ImGUI);
	m_LayerStack.PushLayer(&m_EditorLayer);
	m_LayerStack.PushLayer(&m_CamManager);

	m_VSync.SetName("Lock FPS");

	Entity::C_ComponentManager::Instance();
	auto& rm = Core::C_ResourceManager::Instance();
	rm.RegisterResourceType(new Renderer::TextureLoader());
	rm.RegisterResourceType(new Renderer::MeshLoader());
	rm.RegisterResourceType(new Renderer::TrimeshModelTrimesh());
}

//=================================================================================
C_ExperimentWindow::~C_ExperimentWindow()
{
	static_cast<C_OGLRenderer*>(m_renderer.get())->DestroyControls(m_ImGUI->GetGUIMgr());
	m_CamManager.DestroyControls(m_ImGUI->GetGUIMgr());
	Renderer::C_MaterialManager::Instance().DestroyControls(m_ImGUI->GetGUIMgr());
};

//=================================================================================
void C_ExperimentWindow::Update()
{
	m_EditorLayer.SetCamera(m_CamManager.GetActiveCamera());
	m_ImGUI->FrameBegin();
	m_LayerStack.OnUpdate();
	Core::C_ResourceManager::Instance().UpdatePendingLoads();
	Renderer::C_MaterialManager::Instance().Update();

	auto& tm = Textures::C_TextureUnitManger::Instance();
	tm.Reset();
	Shaders::C_ShaderManager::Instance().Update();
	// MouseSelect();

	const auto avgMsPerFrame = m_Samples.Avg();
	m_GUITexts[::Utils::ToIndex(E_GUITexts::AvgFrametime)].UpdateText(m_Samples.Avg());
	m_GUITexts[::Utils::ToIndex(E_GUITexts::AvgFps)].UpdateText(1000.f / avgMsPerFrame);
	m_GUITexts[::Utils::ToIndex(E_GUITexts::MinMaxFrametime)].UpdateText(*::Utils::min_element(m_Samples), *::Utils::max_element(m_Samples));

	glfwSwapInterval(m_VSync ? 1 : 0);

	if (auto* rayTraceWindow = m_ImGUI->GetGUIMgr().GetWindow(m_RayTraceGUID); rayTraceWindow)
	{
		static_cast<Renderer::C_RayTraceWindow*>(rayTraceWindow)->DebugDraw(&C_DebugDraw::Instance());
	}

	m_World->OnUpdate();

	glfwMakeContextCurrent(m_Window);

	const auto camera = m_CamManager.GetActiveCamera();
	GLE_ASSERT(camera, "No active camera");

	// ======
	// Sun shadow
	// ======
	if (m_SunShadow)
	{
		// shadow pass
		m_SunShadow->Render(*m_World.get(), camera.get());
		m_MainPass->SetSunShadowMap(m_SunShadow->GetZBuffer()->GetHandle());
		m_MainPass->SetSunViewProjection(m_SunShadow->GetLastViewProjection());
	}
	else
	{
		auto& tMGR = Textures::C_TextureManager::Instance();
		m_MainPass->SetSunShadowMap(tMGR.GetIdentityTexture()->GetHandle());
	}
	// ======
	// World render
	// ======
	m_HDRFBO->Bind<E_FramebufferTarget::Draw>();
	m_MainPass->Render(*m_World.get(), camera, GetWidth(), GetHeight());
	{
		RenderDoc::C_DebugScope s("Handles draw");
		m_3DRenderer.Commit(*m_RenderInterfaceHandles.get());
	}
	m_HDRFBO->Unbind<E_FramebufferTarget::Draw>();

	// ======
	// Atmosphere render
	// ======
	Renderer::I_DeviceTexture* HDRTexturePtr = nullptr;
	if (m_HDRFBOAtmosphere)
	{
		m_HDRFBO->Bind<E_FramebufferTarget::Read>();

		const auto			  HDRTexture = m_HDRFBO->GetAttachment(GL_COLOR_ATTACHMENT0);
		const auto			  worldDepth = m_HDRFBO->GetAttachment(GL_DEPTH_ATTACHMENT);
		const FullScreenSetup fsSetup{
			.shaderName = "atmosphere",
			.shaderSetup =
				[](Shaders::C_ShaderProgram& shader) {
					shader.SetUniform("hdrBuffer", 0);
					shader.SetUniform("depthBuffer", 1);
				},
			.inputTextures = {HDRTexture.get(), worldDepth.get()},
			.renderTarget  = *m_HDRFBOAtmosphere.get(),
		};
		m_RenderInterface->RenderFullScreen(fsSetup);
		m_HDRFBO->Unbind<E_FramebufferTarget::Read>();

		HDRTexturePtr = m_HDRFBOAtmosphere->GetAttachment(GL_COLOR_ATTACHMENT0).get();
		m_HDRFBOAtmosphere->Bind<E_FramebufferTarget::Read>();
	}
	else
	{
		m_HDRFBO->Bind<E_FramebufferTarget::Read>();
		HDRTexturePtr = m_HDRFBO->GetAttachment(GL_COLOR_ATTACHMENT0).get();
	}

	// ======
	// HDR correction
	// ======
	auto defaultRenderTarget = m_Device->GetDefualtRendertarget();
	// clear screen
	{
		using namespace Commands;
		m_renderer->AddCommand(std::make_unique<C_GLClear>(C_GLClear::E_ClearBits::Color | C_GLClear::E_ClearBits::Depth));
		m_renderer->AddCommand(std::make_unique<C_GLViewport>(Renderer::C_Viewport(0, 0, GetSize())));
	}
	const FullScreenSetup fsSetup{
		.shaderName = "screenQuad",
		.shaderSetup =
			[gamma = m_GammaSlider.GetValue(), exposure = m_ExposureSlider.GetValue()](Shaders::C_ShaderProgram& shader) {
				shader.SetUniform("gamma", gamma);
				shader.SetUniform("exposure", exposure);
				shader.SetUniform("hdrBuffer", 0);
			},
		.inputTextures = {HDRTexturePtr},
		.renderTarget  = defaultRenderTarget,
	};
	m_RenderInterface->RenderFullScreen(fsSetup);
	m_HDRFBO->Unbind<E_FramebufferTarget::Read>();

	// ======
	// Debug and ImGui
	// ======
	{
		RenderDoc::C_DebugScope s("Persistent debug");
		C_PersistentDebug::Instance().DrawAll();
	}
	{
		RenderDoc::C_DebugScope s("Merged debug");
		C_DebugDraw::Instance().DrawMergedGeoms();
	}
	{
		RenderDoc::C_DebugScope s("ImGUI");
		m_renderer->AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([this]() { m_ImGUI->FrameEnd(m_Input); }, "m_ImGUI->FrameEnd"));
	}

	// commit of final commands - from commit few lines above
	m_renderer->Commit();
	m_renderer->ClearCommandBuffers();
	glfwSwapBuffers(m_Window);
	SampleTime(static_cast<float>(m_FrameTimer.getElapsedTimeFromLastQueryMilliseconds()));
}

//=================================================================================
void C_ExperimentWindow::SampleTime(const double newSample)
{
	m_Samples.Sample(static_cast<float>(newSample));
}

//=================================================================================
void C_ExperimentWindow::OnEvent(Core::I_Event& event)
{
	RenderDoc::C_DebugScope s("OnEvent-Fail");
	// base can make filtering
	T_Base::OnEvent(event);

	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_AppEvent>(std::bind(&C_ExperimentWindow::OnAppEvent, this, std::placeholders::_1));
	d.Dispatch<Core::C_WindowResizedEvent>(std::bind(&C_ExperimentWindow::OnWindowResized, this, std::placeholders::_1));

	m_LayerStack.OnEvent(event);
	m_World->OnEvent(event);
}

//=================================================================================
void C_ExperimentWindow::OnAppInit()
{
	m_FrameTimer.reset();
	m_MainPass = std::make_unique<C_MainPassTechnique>();
	{
		using namespace Commands;
		m_renderer->AddCommand(std::make_unique<C_GLEnable>(C_GLEnable::E_GLEnableValues::DEPTH_TEST));
		m_renderer->AddCommand(std::make_unique<C_GLEnable>(C_GLEnable::E_GLEnableValues::CULL_FACE));
		m_renderer->AddCommand(std::make_unique<C_GLEnable>(C_GLEnable::E_GLEnableValues::BLEND));
		m_renderer->AddCommand(std::make_unique<HACK::C_LambdaCommand>([]() { glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }));
		m_renderer->AddCommand(std::make_unique<C_GLClearColor>(Colours::black));
		m_renderer->AddCommand(std::make_unique<C_GLCullFace>(C_GLCullFace::E_FaceMode::Front));
	}

	Buffers::C_UniformBuffersManager::Instance().CreateUniformBuffer<Buffers::UBO::C_ModelData>("modelData");

	m_RenderInterface
		= std::make_unique<C_RenderInterface>(Shaders::C_ShaderManager::Instance(), Textures::C_TextureUnitManger::Instance(), *static_cast<C_OGLRenderer*>(m_renderer.get()));

	m_RenderInterfaceHandles = std::make_unique<C_GLRenderInterface>();

	SetupWorld("Levels/cornellBox.xml");

#pragma region FBOSteup
	{
		m_HDRFBO = std::unique_ptr<C_Framebuffer>(m_Device->AllocateFramebuffer("HDR"));
		const Renderer::TextureDescriptor HDRTextureDef{"hdrTexture", GetWidth(), GetHeight(), Renderer::E_TextureType::TEXTURE_2D, Renderer::E_TextureFormat::RGBA16f, false};
		auto							  HDRTexture = std::make_shared<Textures::C_Texture>(HDRTextureDef);
		m_Device->AllocateTexture(*HDRTexture.get());

		HDRTexture->SetFilter(Renderer::E_TextureFilter::Linear, Renderer::E_TextureFilter::Linear);
		// ~HDRTexture setup
		m_HDRFBO->AttachTexture(GL_COLOR_ATTACHMENT0, HDRTexture);

		const Renderer::TextureDescriptor hdrDepthTextureDef{
			"hdrDepthTexture", GetWidth(), GetHeight(), Renderer::E_TextureType::TEXTURE_2D, Renderer::E_TextureFormat::D16, false};
		auto depthStencilTexture = std::make_shared<Textures::C_Texture>(hdrDepthTextureDef);
		m_Device->AllocateTexture(*depthStencilTexture.get());

		// depthStencilTexture->SetTexParameter(GL_COMPARE_REF_TO_TEXTURE, GL_COMPARE_REF_TO_TEXTURE);
		// depthStencilTexture->SetTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_NEVER);
		// depthStencilTexture->SetFilter(Renderer::E_TextureFilter::Linear, Renderer::E_TextureFilter::Linear);
		// ~depthStencilTexture setup
		m_HDRFBO->AttachTexture(GL_DEPTH_ATTACHMENT, depthStencilTexture);
	}
#pragma endregion FBOSteup

	auto& guiMGR = m_ImGUI->GetGUIMgr();

	// Console window
	{
		m_ConsoleWindowGUID = NextGUID();

		auto consoleWindow = new GUI::C_ConsoleWindow(m_ConsoleWindowGUID);
		guiMGR.AddCustomWindow(consoleWindow);
		consoleWindow->SetVisible();
	}


	{
		m_EntityEditorGUID = NextGUID();

		auto* entities = new Editor::EntityEditor(m_EntityEditorGUID, guiMGR);

		guiMGR.AddCustomWindow(entities);
		entities->SetVisible();
	}

	// Entity window
	{
		m_EntitiesWindowGUID = NextGUID();

		auto entitiesWindow = new Editor::C_EntitiesWindow(m_EntitiesWindowGUID, m_World);
		guiMGR.AddCustomWindow(entitiesWindow);
		entitiesWindow->SetVisible();
	}

	m_FrameStatsGUID = guiMGR.CreateGUIWindow("Frame stats");
	auto* frameStats = guiMGR.GetWindow(m_FrameStatsGUID);

	frameStats->AddComponent(m_GUITexts[::Utils::ToIndex(E_GUITexts::AvgFrametime)]);
	frameStats->AddComponent(m_GUITexts[::Utils::ToIndex(E_GUITexts::AvgFps)]);
	frameStats->AddComponent(m_GUITexts[::Utils::ToIndex(E_GUITexts::MinMaxFrametime)]);
	frameStats->AddComponent(m_Samples);
	frameStats->AddComponent(m_VSync);
	frameStats->SetVisible(true);
	frameStats->AddMenu(m_Windows);

	m_HDRSettingsGUID = guiMGR.CreateGUIWindow("HDR Settings");
	auto* hdrSettings = guiMGR.GetWindow(m_HDRSettingsGUID);

	hdrSettings->AddComponent(m_GammaSlider);
	hdrSettings->AddComponent(m_ExposureSlider);

	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("New level", [&]() {
		m_World->ClearLevel();
		AddMandatoryWorldParts();
		return true;
	}));

	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Open level", [&]() {
		const auto levelSelectorGUID = NextGUID();
		auto*	   levelSelectWindow = new GUI::C_FileDialogWindow(
			 ".xml", "Select level",
			 [&, levelSelectorGUID](const std::filesystem::path& level, GUI::C_GUIManager& guiMgr) {
				 SetupWorld(level);
				 guiMgr.DestroyWindow(levelSelectorGUID);
			 },
			 levelSelectorGUID, "./Levels");
		guiMGR.AddCustomWindow(levelSelectWindow);
		levelSelectWindow->SetVisible();
		return false;
	}));

	auto& tMGR = Textures::C_TextureManager::Instance();
	tMGR.GetIdentityTexture()->MakeHandleResident(); // Hack, I need to have active renderer because of lack of dependency injection

	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Save Level", [&]() {
		const auto filename = m_World->GetFilename();
		if (filename.empty())
		{
			SaveLevelAs();
		}
		else
		{
			SaveLevel(filename);
		}
		return false;
	}));
	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Save Level As", std::bind(&C_ExperimentWindow::SaveLevelAs, this)));

	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItemOpenWindow>("HDR Settings", m_HDRSettingsGUID, guiMGR));

	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Ray tracing", [&]() {
		if (guiMGR.GetWindow(m_RayTraceGUID) != nullptr)
		{
			return false;
		}
		m_RayTraceGUID = NextGUID();

		auto* rayTraceWindow = new Renderer::C_RayTraceWindow(m_RayTraceGUID, m_CamManager.GetActiveCamera(), guiMGR);

		guiMGR.AddCustomWindow(rayTraceWindow);
		rayTraceWindow->SetVisible(true);
		return false;
	}));

	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Image editor", [&]() {
		if (guiMGR.GetWindow(m_ImageEditorGUID) != nullptr)
		{
			return false;
		}
		m_ImageEditorGUID = NextGUID();

		auto* imageEditorWindow = new Editor::C_ImageEditor(m_ImageEditorGUID, guiMGR);

		guiMGR.AddCustomWindow(imageEditorWindow);
		imageEditorWindow->SetVisible(true);
		return false;
	}));

	const auto rendererWindow = static_cast<C_OGLRenderer*>(m_renderer.get())->SetupControls(guiMGR);
	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItemOpenWindow>("Renderer", rendererWindow, guiMGR));

	const auto camManager = m_CamManager.SetupControls(guiMGR);
	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItemOpenWindow>("Camera manager", camManager, guiMGR));

	const auto materialManager = Renderer::C_MaterialManager::Instance().SetupControls(guiMGR);
	m_Windows.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItemOpenWindow>("Material manager", materialManager, guiMGR));

	CORE_LOG(E_Level::Info, E_Context::Render, "Experiment window setup time was {}", static_cast<float>(m_FrameTimer.getElapsedTimeFromLastQueryMilliseconds()) / 1000.f);
}

//=================================================================================
bool C_ExperimentWindow::OnWindowResized(Core::C_WindowResizedEvent& event)
{
	{
		auto						HDRTexture = m_HDRFBO->GetAttachment(GL_COLOR_ATTACHMENT0);
		Renderer::TextureDescriptor descHDR	   = HDRTexture->GetDescriptor();
		descHDR.width						   = event.GetWidth();
		descHDR.height						   = event.GetHeight();
		m_Device->DestroyTexture(*HDRTexture.get());
		HDRTexture = std::make_shared<Textures::C_Texture>(descHDR);
		m_Device->AllocateTexture(*HDRTexture.get());
		m_HDRFBO->AttachTexture(GL_COLOR_ATTACHMENT0, HDRTexture);
	}

	{
		auto						depthStencilTexture = m_HDRFBO->GetAttachment(GL_DEPTH_ATTACHMENT);
		Renderer::TextureDescriptor descDepth			= depthStencilTexture->GetDescriptor();
		descDepth.width									= event.GetWidth();
		descDepth.height								= event.GetHeight();
		m_Device->DestroyTexture(*depthStencilTexture.get());
		depthStencilTexture = std::make_shared<Textures::C_Texture>(descDepth);
		m_Device->AllocateTexture(*depthStencilTexture.get());
		m_HDRFBO->AttachTexture(GL_DEPTH_ATTACHMENT, depthStencilTexture);
	}

	if (m_HDRFBOAtmosphere)
	{
		auto						HDRTexture = m_HDRFBOAtmosphere->GetAttachment(GL_COLOR_ATTACHMENT0);
		Renderer::TextureDescriptor descHDR	   = HDRTexture->GetDescriptor();
		descHDR.width						   = event.GetWidth();
		descHDR.height						   = event.GetHeight();
		m_Device->DestroyTexture(*HDRTexture.get());
		HDRTexture = std::make_shared<Textures::C_Texture>(descHDR);
		m_Device->AllocateTexture(*HDRTexture.get());
		m_HDRFBOAtmosphere->AttachTexture(GL_COLOR_ATTACHMENT0, HDRTexture);
	}

	m_EditorLayer.SetViewport({0, 0, GetSize()});

	return true;
}

//=================================================================================
void C_ExperimentWindow::SetupWorld(const std::filesystem::path& level)
{
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(level.c_str());
	if (!result.status == pugi::status_ok)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Can't open config file for level name: {}", level);
		return;
	}

	Utils::C_XMLDeserializer d(Core::C_ResourceManager::Instance());
	auto					 newWorld = d.Deserialize<std::shared_ptr<Entity::C_EntityManager>>(doc);
	if (newWorld.has_value() == false)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "XML {} is not valid level.", level);
		return;
	}
	m_World.swap(newWorld.value());
	m_World->SetFilename(level);

	const auto& guiMGR	  = m_ImGUI->GetGUIMgr();
	auto* entitiesWnd = guiMGR.GetWindow(m_EntitiesWindowGUID);
	if (auto* entitiesWindow = dynamic_cast<Editor::C_EntitiesWindow*>(entitiesWnd))
	{
		entitiesWindow->SetWorld(m_World);
	}

	AddMandatoryWorldParts();

	// Inform entities about the level loaded event
	Core::C_EntityEvent levelEvent(GUID::INVALID_GUID, Core::C_EntityEvent::EntityEvent::LevelLoaded);
	m_World->OnEvent(levelEvent);

	// FBO for atmosphere
	if (m_World->GetEntity("atmosphere"))
	{
		m_HDRFBOAtmosphere = std::unique_ptr<C_Framebuffer>(m_Device->AllocateFramebuffer("Atmosphere"));
		const Renderer::TextureDescriptor HDRTextureDef{
			"hdrTextureAtmosphere", GetWidth(), GetHeight(), Renderer::E_TextureType::TEXTURE_2D, Renderer::E_TextureFormat::RGBA16f, false};
		const auto HDRTexture = std::make_shared<Textures::C_Texture>(HDRTextureDef);
		m_Device->AllocateTexture(*HDRTexture.get());

		HDRTexture->SetFilter(Renderer::E_TextureFilter::Linear, Renderer::E_TextureFilter::Linear);
		m_HDRFBOAtmosphere->AttachTexture(GL_COLOR_ATTACHMENT0, HDRTexture);
	}
	else
	{
		m_HDRFBOAtmosphere = nullptr;
	}
}

//=================================================================================
void C_ExperimentWindow::SaveLevel(const std::filesystem::path& filename)
{
	Utils::C_XMLSerializer s;
	const auto			   str = s.Serialize(m_World);
	m_World->SetFilename(filename);
	str.save_file(filename.c_str());
}

//=================================================================================
bool C_ExperimentWindow::SaveLevelAs()
{
	auto&	   guiMGR			 = m_ImGUI->GetGUIMgr();
	const auto levelSelectorGUID = NextGUID();
	auto*	   levelPathSelect	 = new GUI::C_FileDialogWindow(
		   ".xml", "Select level",
		   [&, levelSelectorGUID](const std::filesystem::path& level, GUI::C_GUIManager& guiMgr) {
			   SaveLevel(level);
			   guiMgr.DestroyWindow(levelSelectorGUID);
		   },
		   levelSelectorGUID, "./Levels");
	guiMGR.AddCustomWindow(levelPathSelect);
	levelPathSelect->SetVisible();
	return false;
}

//=================================================================================
void C_ExperimentWindow::AddMandatoryWorldParts()
{
	m_Player = m_World->GetOrCreateEntity("Player");

	auto player = m_Player.lock();
	if (player)
	{
		const auto cameras = player->GetComponents(Entity::E_ComponentType::Camera);
		for (auto i = cameras.size(); i < 2; ++i)
		{
			constexpr float zoom		 = 5.0f;
			auto			playerCamera = std::make_shared<Renderer::Cameras::C_OrbitalCamera>(player);
			playerCamera->setupCameraProjection(0.1f, 2 * zoom * 100, static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()), 90.0f);
			playerCamera->setupCameraView(zoom, glm::vec3(0.0f), 90, 0);
			playerCamera->Update();
			player->AddComponent(playerCamera);
		}
		auto camIt = player->GetComponents(Entity::E_ComponentType::Camera).begin();
		m_CamManager.ActivateCamera(std::static_pointer_cast<Renderer::I_CameraComponent>(*camIt));
		++camIt;
		m_CamManager.SetDebugCamera(std::static_pointer_cast<Renderer::I_CameraComponent>(*camIt));
	}

	{
		// create default atmosphere
		const auto entity = m_World->GetEntity("atmosphere");
		if (entity)
		{
			if (const auto sunLight = entity->GetComponent<Entity::E_ComponentType::Light>())
			{
				m_SunShadow = std::make_shared<C_SunShadowMapTechnique>(std::static_pointer_cast<Renderer::C_SunLight>(sunLight));
			}
			else
			{
				m_SunShadow.reset();
			}
		}
		else
		{
			m_SunShadow.reset();
		}
	}
}

//=================================================================================
void C_ExperimentWindow::MouseSelect()
{
	// this code should go to the editor layer one day
	if (m_ImGUI->CapturingMouse())
	{
		return;
	}

	auto screenCoord = GetInput().GetMousePosition();
	auto camera		 = m_CamManager.GetActiveCamera();

	const auto clipPosition = ToClipSpace({screenCoord.first, screenCoord.second});

	using namespace Physics::Primitives;
	const auto ray = camera->GetRay(clipPosition);

	C_PersistentDebug::Instance().DrawLine(ray.origin, ray.origin + ray.direction, Colours::green);
	const S_RayIntersection inter = m_World->Select(ray);
	if (inter.distance > 0)
	{
		auto entity = m_World->GetEntity(inter.entityId);
		if (entity)
		{
			// todo: entities window needs to know that as well
			Core::C_EntityEvent event(entity->GetID(), Core::C_EntityEvent::EntityEvent::Selected);
			entity->OnEvent(event);
		}
	}
}

//=================================================================================
bool C_ExperimentWindow::OnAppEvent(Core::C_AppEvent& event)
{
	if (event.GetEventType() == Core::C_AppEvent::E_Type::AppInit)
	{
		OnAppInit();
		return true;
	}
	if (event.GetEventType() == Core::C_AppEvent::E_Type::WindowCloseRequest)
	{
		m_World->ClearLevel();
		auto& guiMGR = m_ImGUI->GetGUIMgr();
		guiMGR.DestroyWindow(m_EntitiesWindowGUID);
		guiMGR.DestroyWindow(m_RayTraceGUID);
		guiMGR.DestroyWindow(m_ConsoleWindowGUID);
		guiMGR.DestroyWindow(m_FrameStatsGUID);
		guiMGR.DestroyWindow(m_HDRSettingsGUID);
		m_ImGUI->OnDetach();
	}

	return false;
}

//=================================================================================
bool C_ExperimentWindow::CanClose() const
{
	return m_LayerStack.ReadyForDestroy();
}

} // namespace GLEngine::GLRenderer::Windows