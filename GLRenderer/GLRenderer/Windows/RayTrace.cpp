#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Windows/RayTrace.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/Textures/TextureView.h>

#include <Core/Application.h>

#include <Utils/HighResolutionTimer.h>

#include <imgui.h>

namespace GLEngine::GLRenderer {

constexpr static std::uint16_t s_resolution		 = 512;
constexpr static glm::uvec2	   s_ImageResolution = glm::uvec2(840, 488);
constexpr static unsigned int  s_Coef			 = 1; // 28 max

//=================================================================================
C_RayTraceWindow::C_RayTraceWindow(GUID guid, std::shared_ptr<Renderer::I_CameraComponent> camera)
	: GUI::C_Window(guid, "Ray tracing")
	, m_Camera(camera)
	, m_ImageStorage(s_ImageResolution.x / s_Coef, s_ImageResolution.y / s_Coef, 3)
	, m_SamplesStorage(s_ImageResolution.x / s_Coef, s_ImageResolution.y / s_Coef, 3)
	, m_Image(Textures::C_Texture(Renderer::TextureDescriptor{"rayTrace", s_ImageResolution.x / s_Coef, s_ImageResolution.y/ s_Coef, Renderer::E_TextureType::TEXTURE_2D,
															  Renderer::E_TextureFormat::RGB32f, false}))
	, m_NumCycleSamples(0)
	, m_Running(false)
	, m_RunningCycle(false)
	, m_Renderer(m_Scene)
	, m_DepthSlider(3, 1, 100, "Max path depth")
	, m_GUIImage(m_Image)
{
	auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
	device.AllocateTexture(m_Image);
	m_Image.SetFilter(Renderer::E_TextureFilter::Linear, Renderer::E_TextureFilter::Linear);

	m_GUIImage.SetSize(s_ImageResolution * s_Coef);
}

//=================================================================================
C_RayTraceWindow::~C_RayTraceWindow()
{
	GLE_ASSERT(!IsRunning(), "Raytracing thread is still running.");
	auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
	device.DestroyTexture(m_Image);
}

//=================================================================================
void C_RayTraceWindow::RayTrace()
{
	if (m_Running)
		return;
	std::packaged_task<void()> rayTrace([&]() {
		Utils::HighResolutionTimer renderTime;
		m_Renderer.SetMaxPathDepth(m_DepthSlider);
		m_Renderer.Render(*m_Camera, m_ImageStorage, m_SamplesStorage, & m_ImageLock, m_NumCycleSamples);
		CORE_LOG(E_Level::Warning, E_Context::Render, "Ray trace: {}ms", renderTime.getElapsedTimeFromLastQueryMilliseconds());
		m_Running = false;
		m_NumCycleSamples++;
	});
	m_Running	 = true;
	m_SignalDone = rayTrace.get_future();
	std::thread rtThread(std::move(rayTrace));
	rtThread.detach();
}

//=================================================================================
void C_RayTraceWindow::RunUntilStop()
{
	if (m_Running)
		return;
	m_RunningCycle = m_Running = true;
	std::packaged_task<void()> rayTrace([&]() {
		while (m_RunningCycle)
		{
			Utils::HighResolutionTimer renderTime;
			m_Renderer.SetMaxPathDepth(m_DepthSlider);
			m_Renderer.Render(*m_Camera, m_ImageStorage, m_SamplesStorage, & m_ImageLock, m_NumCycleSamples);
			CORE_LOG(E_Level::Warning, E_Context::Render, "Ray trace: {}ms", renderTime.getElapsedTimeFromLastQueryMilliseconds());
			m_NumCycleSamples++;
		}
		m_Running = false;
	});
	m_SignalDone = rayTrace.get_future();
	std::thread rtThread(std::move(rayTrace));
	rtThread.detach();
}

//=================================================================================
void C_RayTraceWindow::Clear()
{
	glm::vec4 color{0.f, 0.f, 0.f, 0.f};
	Renderer::C_TextureView(&m_ImageStorage).ClearColor(color);
	Renderer::C_TextureView(&m_SamplesStorage).ClearColor(color);
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	renderer.AddTransferCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this]() {
			m_Image.SetTexData2D(0, (&m_ImageStorage));
			m_Renderer.SetResultConsumed();
		},
		"RT buffer"));
	m_NumCycleSamples = 0;
}

//=================================================================================
void C_RayTraceWindow::DrawComponents() const
{
	// This is coming from main thread, we can do updates here
	const_cast<C_RayTraceWindow*>(this)->UploadStorage();
	const auto dim = m_ImageStorage.GetDimensions();
	m_GUIImage.Draw();
	if (!m_Running)
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
		m_DepthSlider.Draw();
	}
	else
	{
		ImGui::ProgressBar(static_cast<float>(m_Renderer.GetProcessedPixels()) / (s_ImageResolution.x * s_ImageResolution.y));

		if (m_RunningCycle)
		{
			if (ImGui::Button("Stop"))
			{
				const_cast<C_RayTraceWindow*>(this)->StopAll();
			}
		}
	}
	ImGui::Text("Samples: %i", m_NumCycleSamples);
}

//=================================================================================
void C_RayTraceWindow::UploadStorage()
{
	// This method should only check whether renderer has any presentable update
	// and if so, than upload it, also this should only be called from main thread
	// so add assert here
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	renderer.AddTransferCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this]() {
			if (m_ImageLock.try_lock())
			{
				if (m_Renderer.NewResultAviable())
				{
					m_Image.SetTexData2D(0, (&m_ImageStorage));
					m_Renderer.SetResultConsumed();
				}
				m_ImageLock.unlock();
			}
		},
		"RT buffer"));
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

} // namespace GLEngine::GLRenderer