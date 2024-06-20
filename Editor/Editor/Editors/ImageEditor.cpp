#include <EditorStdafx.h>

#include <Editor/Editors/Image/Tools/BrickGenerator.h>
#include <Editor/Editors/ImageEditor.h>
#include <Editor/Editors/ImageEditorTool.h>

#include <Renderer/Colours.h>
#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Render/CPURasterizer.h>
#include <Renderer/Textures/TextureView.h>

#include <GUI/FileDialogWindow.h>
#include <GUI/GUIManager.h>
#include <GUI/Menu/Menu.h>
#include <GUI/Menu/MenuItem.h>
#include <GUI/ReflectionGUI.h>

#include <Core/Application.h>

#include <Utils/HighResolutionTimer.h>

#include <imgui_internal.h>

namespace GLEngine::Editor {

const static glm::vec2	s_ImageDrawArea{800, 800};
const static glm::uvec2 s_BackgroundDim{2, 2};

//=================================================================================
C_ImageEditor::C_ImageEditor(GUID guid, GUI::C_GUIManager& guiMGR)
	: GUI::C_Window(guid, "Image editor")
	, m_Storage(1024, 1024, 4)
	, m_GUIImage(nullptr)
	, m_FileMenu("File")
	, m_Tools("Tools")
	, m_DeviceImage(nullptr)
	, m_Background(nullptr)
{
	auto&							  device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
	const Renderer::TextureDescriptor desc{.name		  = "Editor image",
										   .width		  = 1024,
										   .height		  = 1024,
										   .type		  = Renderer::E_TextureType::TEXTURE_2D,
										   .format		  = Renderer::E_TextureFormat::RGBA32f,
										   .m_bStreamable = false};
	const Renderer::TextureDescriptor descTransparent{.name			 = "Transparent background",
													  .width		 = s_BackgroundDim.x,
													  .height		 = s_BackgroundDim.y,
													  .type			 = Renderer::E_TextureType::TEXTURE_2D,
													  .format		 = Renderer::E_TextureFormat::RGB32f,
													  .m_bStreamable = false};

	m_DeviceImage = device.CreateTextureHandle(desc);
	m_Background  = device.CreateTextureHandle(descTransparent);
	device.AllocateTexture(*m_DeviceImage.get());
	device.AllocateTexture(*m_Background.get());
	m_Storage.SetAll(glm::vec4(Colours::white, 0.f));
	{
		Renderer::C_TextureViewStorageCPU<float> storage(s_BackgroundDim.x, s_BackgroundDim.y, 3);
		storage.SetAll(glm::vec4(Colours::white, 0.f));
		Renderer::C_TextureView view(&storage);

		view.Set(glm::ivec2{0, 0}, glm::vec3{Colours::gray});
		view.Set(glm::ivec2{1, 1}, glm::vec3{Colours::gray});

		m_Background->SetTexData2D(0, &storage);
		m_Background->SetWrap(Renderer::E_WrapFunction::Repeat, Renderer::E_WrapFunction::Repeat);
		m_Background->SetFilter(Renderer::E_TextureFilter::Nearest, Renderer::E_TextureFilter::Nearest);
	}

	AddMenu(m_FileMenu);
	AddMenu(m_Tools);

	m_Tools.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Histogram", std::bind(&C_ImageEditor::ToggleHistogram, this)));
	m_Tools.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Brick", [&]() { m_ActiveTool = std::make_unique<C_BrickGenerator>(Renderer::C_TextureView(&m_Storage)); }));
	m_Tools.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Blur", [&]() { m_ActiveTool = std::make_unique<C_GaussianBlur>(Renderer::C_TextureView(&m_Storage)); }));
	std::reference_wrapper<GUI::Menu::C_MenuItem> createMenuItem = guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Save as...", [&]() {
		const auto textureSelectorGUID = NextGUID();
		auto*	   textureSelectWindow = new GUI::C_FileDialogWindow(
			 ".bmp,.hdr,.ppm", "Save image as...",
			 [&, textureSelectorGUID](const std::filesystem::path& texture) {
				 // TODO save here

				 guiMGR.DestroyWindow(textureSelectorGUID);
			 },
			 textureSelectorGUID, "./Images");
		guiMGR.AddCustomWindow(textureSelectWindow);
		textureSelectWindow->SetVisible();
	});
	m_FileMenu.AddMenuItem(createMenuItem);

	m_GUIImage = new GUI::C_ImageViewer(*m_DeviceImage.get());
	m_GUIImage->SetSize({800, 800});
	m_DeviceImage->SetTexData2D(0, &m_Storage);

	std::thread([&]() {
		Utils::HighResolutionTimer timer;
		timer.reset();
		Renderer::C_TextureView view(&m_Storage);
		view.EnableBlending();
		int x1 = 100;
		int x2 = 900;
		int y1 = 100;
		int y2 = 900;

		glm::uvec2				  center{512, 512};
		Renderer::C_CPURasterizer rasterizer(view);
		rasterizer.DrawCircle(Colours::red, center, 200);
		rasterizer.DrawCircle(Colours::red, center, 400);
		// rasterizer.FloodFill(Colours::blue, {212, 512});
		// rasterizer.FloodFill(Colours::green, {512, 512});
		// for (int i = 0; i < 36; ++i)
		//{
		//	glm::ivec2 dir{std::cos(glm::radians(i * 10.f)) * 400, std::sin(glm::radians(i * 10.f)) * 400};
		//	rasterizer.DrawLine(Colours::red, center, glm::ivec2{center} + dir, true);
		//}
		m_bDone		= true;
		m_bModified = true;
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
	const ImVec2 canvas_p0 = ImGui::GetCursorPos();
	ImGui::Image((void*)(intptr_t)(m_Background->GetGPUHandle()), {s_ImageDrawArea.x, s_ImageDrawArea.y}, {0, 0},
				 {s_ImageDrawArea.x / (s_BackgroundDim.x * 5), s_ImageDrawArea.y / (s_BackgroundDim.y * 5)});
	ImGui::SetCursorPos(canvas_p0);
	m_GUIImage->Draw();
	::ImGui::SameLine();
	if (m_ActiveTool)
	{
		::ImGui::BeginChild("ImageTools");
		rttr::instance obj(m_ActiveTool.get());
		if (GUI::DrawAllPropertyGUI(obj).empty() == false)
		{
			
		}
		if (ImGui::Button("Apply")) {
			m_ActiveTool->Apply();
			m_bFinish = false; // hack
		}
		::ImGui::EndChild();
	}
}

void C_ImageEditor::ToggleHistogram()
{
	CORE_LOG(E_Level::Error, E_Context::Core, "Histogram.");
}

void C_ImageEditor::SetupToolPreview()
{
}

} // namespace GLEngine::Editor