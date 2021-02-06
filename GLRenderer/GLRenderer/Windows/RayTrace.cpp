#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Windows/RayTrace.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/IRenderer.h>
#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/Textures/TextureView.h>

#include <Core/Application.h>

#include <Utils/HighResolutionTimer.h>

#include <imgui.h>

namespace GLEngine::GLRenderer {

constexpr static std::uint16_t s_resolution = 512;

//=================================================================================
C_RayTraceWindow::C_RayTraceWindow(GUID guid, std::shared_ptr<Renderer::I_CameraComponent> camera)
	: GUI::C_Window(guid, "Ray tracing")
	, m_Camera(camera)
	, m_ImageStorage(844, 480, 3)
	, m_Image(nullptr)
	, m_NumCycleSamples(0)
	, m_Running(false)
	, m_RunningCycle(false)
	, m_LiveUpdate(false, "Live update")
	, m_DirectionImage(s_resolution, s_resolution, 1)
	, m_DirImage(nullptr)
{
	{
		m_Image		  = std::make_shared<Textures::C_Texture>("rayTrace");
		auto channels = m_ImageStorage.GetChannels();
		channels[3]	  = Renderer::E_TextureChannel::None;
		m_ImageStorage.SetChannels(channels);
	}

	{
		auto channels = m_DirectionImage.GetChannels();
		channels[1]	  = Renderer::E_TextureChannel::None;
		channels[2]	  = Renderer::E_TextureChannel::None;
		channels[3]	  = Renderer::E_TextureChannel::None;
		m_DirectionImage.SetChannels(channels);

		m_DirImage = std::make_shared<Textures::C_Texture>("directional");
		m_DirImage->bind();
		m_DirImage->SetTexData2D(0, &m_DirectionImage);
		m_DirImage->unbind();
	}
}

//=================================================================================
C_RayTraceWindow::~C_RayTraceWindow()
{
	GLE_ASSERT(!IsRunning(), "Raytracing thread is still running.");
}

//=================================================================================
void C_RayTraceWindow::RayTrace()
{
	if (m_Running)
		return;
	std::packaged_task<void()> rayTrace([&]() {
		Renderer::C_RayRenderer	   renderer(m_Scene);
		Utils::HighResolutionTimer renderTime;
		renderer.SetDirectionalDebug(Renderer::C_OctahedralTextureView(Renderer::C_TextureView(&m_DirectionImage), s_resolution));
		renderer.Render(*m_Camera, m_ImageStorage);
		CORE_LOG(E_Level::Warning, E_Context::Render, "Ray trace: {}ms", renderTime.getElapsedTimeFromLastQueryMilliseconds());
		UploadStorage();
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
		Renderer::C_RayRenderer renderer(m_Scene);
		while (m_RunningCycle)
		{
			Utils::HighResolutionTimer renderTime;
			renderer.Render(*m_Camera, m_ImageStorage);
			CORE_LOG(E_Level::Warning, E_Context::Render, "Ray trace: {}ms", renderTime.getElapsedTimeFromLastQueryMilliseconds());
			UploadStorage();
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
	Renderer::C_TextureView(&m_DirectionImage).ClearColor(color);
	UploadStorage();
	m_NumCycleSamples = 0;
}

//=================================================================================
void C_RayTraceWindow::DrawComponents() const
{
	if (m_LiveUpdate && m_Running)
		UploadStorage();
	const auto dim = m_ImageStorage.GetDimensions();
	ImGui::Image((void*)(intptr_t)(m_Image->GetTexture()), ImVec2(static_cast<float>(dim.x), static_cast<float>(dim.y)));
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
	}
	if (m_RunningCycle)
	{
		if (ImGui::Button("Stop"))
		{
			const_cast<C_RayTraceWindow*>(this)->StopAll();
		}
	}
	ImGui::Text("Samples: %i", m_NumCycleSamples);
	ImGui::Image((void*)(intptr_t)(m_DirImage->GetTexture()), ImVec2(static_cast<float>(256), static_cast<float>(256)));
	m_LiveUpdate.Draw();
}

//=================================================================================
void C_RayTraceWindow::UploadStorage() const
{
	bool foundRenderer = false;
	while (foundRenderer == false)
	{
		const auto& renderer = Core::C_Application::Get().GetActiveRenderer();
		if (renderer)
		{
			renderer->AddTransferCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
				[this]() {
					m_Image->bind();
					m_Image->SetTexData2D(0, (&m_ImageStorage));
					m_Image->GenerateMipMaps();
				},
				"RT buffer"));
			renderer->AddTransferCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
				[this]() {
					m_DirImage->bind();
					m_DirImage->SetTexData2D(0, (&m_DirectionImage));
					m_DirImage->GenerateMipMaps();
					m_DirImage->unbind();
				},
				"RT buffer"));
			foundRenderer = true;
		}
	}
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
	StopAll();
}

//=================================================================================
bool C_RayTraceWindow::CanDestroy() const
{
	return !IsRunning();
}

} // namespace GLEngine::GLRenderer