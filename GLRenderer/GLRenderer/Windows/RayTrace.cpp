#include <GLRendererStdafx.h>

#include <GLRenderer/Windows/RayTrace.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Textures/Texture.h>

#include <Renderer/IRenderer.h>
#include <Renderer/ICameraComponent.h>
#include <Renderer/RayCasting/RayRenderer.h>

#include <Core/Application.h>

#include <Utils/HighResolutionTimer.h>

#include <imgui.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_RayTraceWindow::C_RayTraceWindow(GUID guid, std::shared_ptr<Renderer::I_CameraComponent> camera)
	: GUI::C_Window(guid, "Ray tracing")
	, m_Camera(camera)
	, m_ImageStorage(580, 480, 3)
	, m_Image(nullptr)
	, m_Running(false)
{
	m_Image = std::make_shared<Textures::C_Texture>("rayTrace");
	auto channels = m_ImageStorage.GetChannels();
	channels[3] = Renderer::E_TextureChannel::None;
	m_ImageStorage.SetChannels(channels);
}

//=================================================================================
void C_RayTraceWindow::RayTrace()
{
	if (m_Running) return;
	std::packaged_task<void()> rayTrace([&]() {
		Renderer::C_RayRenderer renderer(m_Scene);
		Utils::HighResolutionTimer renderTime;
		renderer.Render(*m_Camera, m_ImageStorage);
		CORE_LOG(E_Level::Warning, E_Context::Render, "Ray trace: {}ms", renderTime.getElapsedTimeFromLastQueryMilliseconds());
		UploadStorage();
		m_Running = false;
		});
	m_Running = true;
	m_SignalDone = rayTrace.get_future();
	std::thread rtThread(std::move(rayTrace));
	rtThread.detach();
}

//=================================================================================
void C_RayTraceWindow::Clear()
{

}

//=================================================================================
void C_RayTraceWindow::DrawComponents() const
{
	const auto dim = m_ImageStorage.GetDimensions();
	ImGui::Image((void*)(intptr_t)(m_Image->GetTexture()), ImVec2(static_cast<float>(dim.x), static_cast<float>(dim.y)));
	if (!m_Running)
	{
		if (ImGui::Button("Render"))
		{
			const_cast<C_RayTraceWindow*>(this)->RayTrace();
		}
		if (ImGui::Button("Clear"))
		{
			const_cast<C_RayTraceWindow*>(this)->Clear();
		}
	}
}

//=================================================================================
void C_RayTraceWindow::UploadStorage()
{
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::make_unique<Commands::HACK::C_LambdaCommand>(
			[this]() {
				m_Image->bind();
				m_Image->SetTexData2D(0, (&m_ImageStorage));
				m_Image->GenerateMipMaps();
			}, "RT buffer"
			)
	);
}

}