#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Windows/RayTrace.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureView.h>

#include <GUI/FileDialogWindow.h>
#include <GUI/GUIManager.h>

#include <Core/Application.h>

#include <Utils/HighResolutionTimer.h>

#include <imgui.h>

namespace GLEngine::GLRenderer {

constexpr static std::uint16_t s_resolution		 = 512;
constexpr static glm::uvec2	   s_ImageResolution = glm::uvec2(840, 488);
constexpr static unsigned int  s_Coef			 = 1; // 28 max
constexpr static unsigned int  s_ProbeSize		 = 64;

//=================================================================================
C_RayTraceWindow::C_RayTraceWindow(GUID guid, std::shared_ptr<Renderer::I_CameraComponent> camera, GUI::C_GUIManager& guiMGR)
	: GUI::C_Window(guid, "Ray tracing")
	, m_Camera(camera)
	, m_ImageStorage(s_ImageResolution.x / s_Coef, s_ImageResolution.y / s_Coef, 3)
	, m_ProbeStorage(s_ProbeSize + 2, s_ProbeSize + 2, 3)
	, m_SamplesStorage(s_ImageResolution.x / s_Coef, s_ImageResolution.y / s_Coef, 3)
	, m_HeatMapStorage(1, s_ImageResolution.y / s_Coef, 1)
	, m_HeatMapNormalizedStorage(1, s_ImageResolution.y / s_Coef, 3)
	, m_Image(Textures::C_Texture(Renderer::TextureDescriptor{.name			 = "rayTrace",
															  .width		 = s_ImageResolution.x / s_Coef,
															  .height		 = s_ImageResolution.y / s_Coef,
															  .type			 = Renderer::E_TextureType::TEXTURE_2D,
															  .format		 = Renderer::E_TextureFormat::RGB32f,
															  .m_bStreamable = false}))
	, m_HeatMap(Textures::C_Texture(Renderer::TextureDescriptor{.name		   = "heatMap",
																.width		   = 1,
																.height		   = s_ImageResolution.y / s_Coef,
																.type		   = Renderer::E_TextureType::TEXTURE_2D,
																.format		   = Renderer::E_TextureFormat::RGB32f,
																.m_bStreamable = false}))
	, m_Probe(std::make_shared<Textures::C_Texture>(Renderer::TextureDescriptor{.name		  = "probeTexture",
																				.width		  = s_ProbeSize + 2,
																				.height		  = s_ProbeSize + 2,
																				.type		  = Renderer::E_TextureType::TEXTURE_2D,
																				.format		  = Renderer::E_TextureFormat::RGB32f,
																				.m_NumSamples = false}))
	, m_NumCycleSamples(0)
	, m_Running(false)
	, m_RunningCycle(false)
	, m_Scene()
	, m_Renderer(nullptr)
	, m_ProbeRenderer(nullptr)
	, m_DepthSlider(3, 1, 100, "Max path depth")
	, m_GUIImage(m_Image)
	, m_GUIHeatMapImage(m_HeatMap)
	, m_GUIImageProbe(*m_Probe.get())
	, m_FileMenu("File")
	, m_DebugDraw(false, "Debug draw")
	, m_ProbePosition("Probe position", glm::vec3(1.f, 0.f, 0.f))
{
	auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
	device.AllocateTexture(m_Image);
	device.AllocateTexture(m_HeatMap);
	device.AllocateTexture(*m_Probe.get());
	m_Image.SetFilter(Renderer::E_TextureFilter::Linear, Renderer::E_TextureFilter::Linear);
	m_HeatMap.SetFilter(Renderer::E_TextureFilter::Nearest, Renderer::E_TextureFilter::Nearest);
	m_Probe->SetFilter(Renderer::E_TextureFilter::Linear, Renderer::E_TextureFilter::Linear);

	m_GUIImage.SetSize(s_ImageResolution);
	m_GUIHeatMapImage.SetSize({10, s_ImageResolution.y});
	m_GUIImageProbe.SetSize({128, 128});

	AddMenu(m_FileMenu);

	m_FileMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Save as...", [&]() {
		const auto textureSelectorGUID = NextGUID();
		auto*	   textureSelectWindow = new GUI::C_FileDialogWindow(
			 ".bmp,.hdr,.ppm", "Save image as...",
			 [&, textureSelectorGUID](const std::filesystem::path& texture) {
				 SaveCurrentImage(texture);
				 guiMGR.DestroyWindow(textureSelectorGUID);
			 },
			 textureSelectorGUID, "./Images");
		guiMGR.AddCustomWindow(textureSelectWindow);
		textureSelectWindow->SetVisible();
	}));
}

//=================================================================================
C_RayTraceWindow::~C_RayTraceWindow()
{
	GLE_ASSERT(!IsRunning(), "Raytracing thread is still running.");
	auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
	device.DestroyTexture(m_Image);
	device.DestroyTexture(m_HeatMap);
	device.DestroyTexture(*m_Probe.get());
}

//=================================================================================
void C_RayTraceWindow::RayTrace()
{
	GLE_ASSERT(m_Renderer, "Ray tracing running during load.");
	if (m_Running)
		return;
	std::packaged_task<void()> rayTrace([&]() {
		Utils::HighResolutionTimer renderTime;
		m_Renderer->SetMaxPathDepth(m_DepthSlider);
		m_Renderer->Render(*m_Camera, m_ImageStorage, m_SamplesStorage, &m_ImageLock, m_NumCycleSamples, {.rowHeatMap = &m_HeatMapStorage});
		CORE_LOG(E_Level::Warning, E_Context::Render, "Ray trace: {}ms", renderTime.getElapsedTimeFromLastQueryMilliseconds());
		m_Running = false;
		RecalculateHeatMap();
		m_NumCycleSamples++;
	});
	m_Running = true;
	std::thread rtThread(std::move(rayTrace));
	rtThread.detach();
}

//=================================================================================
void C_RayTraceWindow::RayTraceProbe()
{
	GLE_ASSERT(m_Renderer, "Ray tracing running during load.");
	if (m_Running)
		return;
	std::packaged_task<void()> rayTrace([&]() {
		Utils::HighResolutionTimer renderTime;
		m_ProbeRenderer->Render(m_ProbeStorage, GetProbePosition(), nullptr);
		CORE_LOG(E_Level::Warning, E_Context::Render, "Ray trace probe: {}ms", renderTime.getElapsedTimeFromLastQueryMilliseconds());
		m_Running = false;
	});
	m_Running = true;
	std::thread rtThread(std::move(rayTrace));
	rtThread.detach();
}

//=================================================================================
void C_RayTraceWindow::RunUntilStop()
{
	GLE_ASSERT(m_Renderer, "Ray tracing running during load.");

	if (m_Running)
		return;
	m_RunningCycle = m_Running = true;
	std::packaged_task<void()> rayTrace([&]() {
		while (m_RunningCycle)
		{
			Utils::HighResolutionTimer renderTime;
			m_Renderer->SetMaxPathDepth(m_DepthSlider);
			m_Renderer->Render(*m_Camera, m_ImageStorage, m_SamplesStorage, &m_ImageLock, m_NumCycleSamples);
			CORE_LOG(E_Level::Warning, E_Context::Render, "Ray trace: {}ms", renderTime.getElapsedTimeFromLastQueryMilliseconds());
			m_NumCycleSamples++;
			RecalculateHeatMap();
		}
		m_Running = false;
	});
	std::thread				   rtThread(std::move(rayTrace));
	rtThread.detach();
}

//=================================================================================
void C_RayTraceWindow::Clear()
{
	GLE_ASSERT(m_Renderer, "Ray tracing running during load.");

	glm::vec4 color{0.f, 0.f, 0.f, 0.f};
	Renderer::C_TextureView(&m_ImageStorage).ClearColor(color);
	Renderer::C_TextureView(&m_HeatMapStorage).ClearColor(color);
	Renderer::C_TextureView(&m_HeatMapNormalizedStorage).ClearColor(color);
	Renderer::C_TextureView(&m_SamplesStorage).ClearColor(color);
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	renderer.AddTransferCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this]() {
			m_Image.SetTexData2D(0, (&m_ImageStorage));
			m_HeatMap.SetTexData2D(0, (&m_HeatMapNormalizedStorage));
			m_Renderer->SetResultConsumed();
		},
		"RT buffer"));
	m_NumCycleSamples = 0;
}

//=================================================================================
void C_RayTraceWindow::Update()
{
	// This is coming from main thread, we can do updates here
	if (StillLoadingScene())
	{
		// try for the complete result
		if (m_Scene.IsLoaded())
		{
			m_Scene.BuildScene();
			m_Renderer		= std::make_unique<Renderer::C_RayRenderer>(m_Scene);
			m_ProbeRenderer = std::make_unique<Renderer::C_ProbeRenderer>(m_Scene, 64);
		}
	}
	// simply destroy when closed
	if (!IsVisible() && !IsRunning())
	{
		m_WantToBeDestroyed = true;
		return;
	}
	UploadStorage();
}

//=================================================================================
void C_RayTraceWindow::DrawComponents() const
{
	const auto dim = m_ImageStorage.GetDimensions();
	m_GUIImage.Draw();
	ImGui::SameLine();
	m_GUIHeatMapImage.Draw();
	m_GUIImageProbe.Draw();
	m_ProbePosition.Draw();
	if (StillLoadingScene())
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Still loading scene.");
	}
	else if (!m_Running)
	{
		if (ImGui::Button("Render"))
		{
			const_cast<C_RayTraceWindow*>(this)->RayTrace();
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			const_cast<C_RayTraceWindow*>(this)->Clear();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cycle"))
		{
			const_cast<C_RayTraceWindow*>(this)->RunUntilStop();
		}
		ImGui::SameLine();
		if (ImGui::Button("Render probe"))
		{
			const_cast<C_RayTraceWindow*>(this)->RayTraceProbe();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset probe"))
		{
			m_ProbeRenderer->ResetProbe();
		}
		m_DepthSlider.Draw();
	}
	else
	{
		ImGui::ProgressBar(static_cast<float>(m_Renderer->GetProcessedPixels()) / (s_ImageResolution.x * s_ImageResolution.y));

		if (m_RunningCycle)
		{
			if (ImGui::Button("Stop"))
			{
				const_cast<C_RayTraceWindow*>(this)->StopAll();
			}
		}
	}
	ImGui::Text("Samples: %i", m_NumCycleSamples);
	m_DebugDraw.Draw();
}

//=================================================================================
void C_RayTraceWindow::UploadStorage()
{
	// not yet initialized
	if (!m_Renderer)
		return;
	// This method should only check whether renderer has any presentable update
	// and if so, than upload it, also this should only be called from main thread
	// so add assert here
	if (m_Renderer->NewResultAviable())
		RecalculateHeatMap();
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	renderer.AddTransferCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this]() {
			if (m_ImageLock.try_lock())
			{
				if (m_Renderer->NewResultAviable())
				{
					m_Image.SetTexData2D(0, (&m_ImageStorage));
					m_HeatMap.SetTexData2D(0, (&m_HeatMapNormalizedStorage));
					m_Renderer->SetResultConsumed();
				}
				m_ImageLock.unlock();
			}
		},
		"RT buffer"));
	renderer.AddTransferCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this]() {
			if (m_ProbeRenderer->NewResultAviable())
			{
				m_Probe->SetTexData2D(0, (&m_ProbeStorage));
				m_ProbeRenderer->SetResultConsumed();
			}
		},
		"RT Probe buffer"));
}

//=================================================================================
void C_RayTraceWindow::StopAll()
{
	m_RunningCycle = false;
}

//=================================================================================
bool C_RayTraceWindow::IsRunning() const
{
	return m_Running;
}

//=================================================================================
void C_RayTraceWindow::RequestDestroy()
{
	GUI::C_Window::RequestDestroy();
	StopAll();
}

//=================================================================================
bool C_RayTraceWindow::CanDestroy() const
{
	return !IsRunning();
}

//=================================================================================
void C_RayTraceWindow::SaveCurrentImage(const std::filesystem::path& texture)
{
	Renderer::Textures::TextureLoader loader;
	if (!loader.SaveTexture(texture, &m_ImageStorage))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Ray tracer cannot save the image.");
	}
}

//=================================================================================
void C_RayTraceWindow::DebugDraw(Renderer::I_DebugDraw* dd) const
{
	if (m_DebugDraw)
		m_Scene.DebugDraw(dd);
}

//=================================================================================
bool C_RayTraceWindow::StillLoadingScene() const
{
	return m_Renderer == nullptr;
}

//=================================================================================
void C_RayTraceWindow::RecalculateHeatMap()
{
	const Renderer::C_TextureView heatMap(&m_HeatMapStorage);
	Renderer::C_TextureView		  result(&m_HeatMapNormalizedStorage);
	float min = std::numeric_limits<float>::max();
	float max = std::numeric_limits<float>::min();
	for (unsigned int row = 0; row < heatMap.GetDimensions().y; ++row)
	{
		const auto sample = heatMap.Get<float>(glm::ivec2{0, row}, Renderer::E_TextureChannel::Red);
		if (sample < min)
			min = sample;
		if (sample > max)
			max = sample;
	}
	const float interval = max - min;
	const Colours::T_Colour minColour = Colours::green;
	const Colours::T_Colour maxColour = Colours::red;
	for (unsigned int row = 0; row < heatMap.GetDimensions().y; ++row)
	{
		const auto sample = heatMap.Get<float>(glm::ivec2{0, row}, Renderer::E_TextureChannel::Red);
		result.Set(glm::ivec2{0, row}, glm::mix(minColour, maxColour, (sample - min) / interval));
	}
}

} // namespace GLEngine::GLRenderer