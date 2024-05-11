#include <EditorStdafx.h>

#include <Editor/Editors/ImageEditor.h>

#include <Renderer/Colours.h>
#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Render/CPURasterizer.h>
#include <Renderer/Textures/TextureView.h>

#include <Core/Application.h>

#include <Utils/HighResolutionTimer.h>

namespace GLEngine::Editor {

//=================================================================================
C_ImageEditor::C_ImageEditor(GUID guid)
	: GUI::C_Window(guid, "Image editor")
	, m_Storage(1024, 1024, 3)
	, m_GUIImage(nullptr)
	, m_DeviceImage(nullptr)
{
	auto&							  device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
	const Renderer::TextureDescriptor desc{.name		  = "Editor image",
										   .width		  = 1024,
										   .height		  = 1024,
										   .type		  = Renderer::E_TextureType::TEXTURE_2D,
										   .format		  = Renderer::E_TextureFormat::RGB32f,
										   .m_bStreamable = false};

	m_DeviceImage = device.CreateTextureHandle(desc);
	device.AllocateTexture(*m_DeviceImage.get());
	m_Storage.SetAll(glm::vec4(Colours::white, 0.f));

	m_GUIImage = new GUI::C_ImageViewer(*m_DeviceImage.get());
	m_GUIImage->SetSize({800, 800});
	m_DeviceImage->SetTexData2D(0, &m_Storage);


	std::thread([&]() {
		Utils::HighResolutionTimer timer;
		timer.reset();
		Renderer::C_TextureView view(&m_Storage);
		view.EnableBlending();
		int						x1 = 100;
		int						x2 = 900;
		int						y1 = 100;
		int						y2 = 900;

		glm::uvec2				  center{512, 512};
		Renderer::C_CPURasterizer rasterizer(view);
		rasterizer.DrawCircle(Colours::red, center, 200);
		rasterizer.DrawCircle(Colours::red, center, 400);
		rasterizer.FloodFill(Colours::blue, {212, 512});
		//rasterizer.FloodFill(Colours::green, {512, 512});
		//for (int i = 0; i < 36; ++i)
		//{
		//	glm::ivec2 dir{std::cos(glm::radians(i * 10.f)) * 400, std::sin(glm::radians(i * 10.f)) * 400};
		//	rasterizer.DrawLine(Colours::red, center, glm::ivec2{center} + dir, true);
		//}
		m_bDone = true;
		CORE_LOG(E_Level::Info, E_Context::Render, "Line render time {}", float(timer.getElapsedTimeFromLastQueryMilliseconds()) / 1000.f);
	}).detach();
}

//=================================================================================
C_ImageEditor::~C_ImageEditor()
{
	delete m_GUIImage;
	auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
	device.DestroyTexture(*m_DeviceImage.get());
}

//=================================================================================
void C_ImageEditor::Update()
{
	static int numUpdates = 0;
	if (!m_bFinish)
	{
		++numUpdates;
		if (m_bDone)
		{
			CORE_LOG(E_Level::Info, E_Context::Render, "Updates: {}", numUpdates);
		}
			m_DeviceImage->SetTexData2D(0, &m_Storage); // possibly miss last update
		m_bFinish = m_bDone;
	}
}

//=================================================================================
void C_ImageEditor::DrawComponents() const
{
	m_GUIImage->Draw();
}

} // namespace GLEngine::Editor