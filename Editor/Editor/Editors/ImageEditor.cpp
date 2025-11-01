#include <EditorStdafx.h>

#include <Editor/Editors/Image/Tools/BrickGenerator.h>
#include <Editor/Editors/Image/Tools/WaveGenerator.h>
#include <Editor/Editors/ImageEditor.h>
#include <Editor/Editors/ImageEditorTool.h>

#include <Renderer/Colours.h>
#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Render/CPURasterizer.h>
#include <Renderer/Resources/ResourceManager.h>
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
static constexpr glm::uvec2 s_BackgroundDim{2, 2};

//=================================================================================
C_ImageEditor::C_ImageEditor(GUID guid, GUI::C_GUIManager& guiMGR)
	: GUI::C_Window(guid, "Image editor")
	, m_Storage(1024, 1024, 4)
	, m_GUIImage({})
	, m_FileMenu("File")
	, m_Tools("Tools")
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	CreateTextures(renderer);

	AddMenu(m_FileMenu);
	AddMenu(m_Tools);

	m_Tools.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Histogram", std::bind(&C_ImageEditor::ToggleHistogram, this)));
	m_Tools.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Brick", [&]() {
		m_ActiveTool = std::make_unique<C_BrickGenerator>(Renderer::C_TextureView(&m_Storage));
		return true;
	}));
	m_Tools.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Blur", [&]() {
		m_ActiveTool = std::make_unique<C_GaussianBlur>(Renderer::C_TextureView(&m_Storage));
		return true;
	}));
	m_Tools.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Wave", [&]() {
		m_ActiveTool = std::make_unique<C_WaveGenerator>(Renderer::C_TextureView(&m_Storage));
		return true;
	}));
	std::reference_wrapper<GUI::Menu::C_MenuItem> createMenuItem = guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>(std::string("Save as..."), [&]() {
		const auto textureSelectorGUID = NextGUID();
		auto*	   textureSelectWindow = new GUI::C_FileDialogWindow(
			 ".bmp,.hdr,.ppm", "Save image as...",
			 [&, textureSelectorGUID](const std::filesystem::path& texture, GUI::C_GUIManager& guiMgr) {
				 // TODO save here
				 guiMgr.DestroyWindow(textureSelectorGUID);
			 },
			 textureSelectorGUID, "./Images");
		guiMGR.AddCustomWindow(textureSelectWindow);
		textureSelectWindow->SetVisible();
		return false;
	});
	m_FileMenu.AddMenuItem(createMenuItem);

	m_GUIImage = GUI::C_ImageViewer(m_DeviceImage);
	m_GUIImage.SetSize({800, 800});
	m_GUIImage.SetBackground(m_Background, {1. / (s_BackgroundDim.x * 15.f), 1. / (s_BackgroundDim.x * 15.f)});
	renderer.SetTextureData(m_DeviceImage, m_Storage);

	std::thread([&]() {
		::Utils::HighResolutionTimer timer;
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
		CORE_LOG(E_Level::Info, E_Context::Render, "Line render time {}", static_cast<float>(timer.getElapsedTimeFromLastQueryMilliseconds()) / 1000.f);
	}).detach();
}

//=================================================================================
C_ImageEditor::~C_ImageEditor()
{
	auto& rm = Core::C_Application::Get().GetActiveRenderer().GetRM();
	rm.destoryTexture(m_DeviceImage);
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
		auto& renderer = Core::C_Application::Get().GetActiveRenderer();
		renderer.SetTextureData(m_DeviceImage, m_Storage);
		m_bFinish = m_bDone;
	}
}

//=================================================================================
void C_ImageEditor::DrawComponents() const
{
	m_GUIImage.Draw();
	::ImGui::SameLine();
	if (m_ActiveTool)
	{
		::ImGui::BeginChild("ImageTools");
		rttr::instance obj(m_ActiveTool.get());
		if (GUI::DrawAllPropertyGUI(obj).empty() == false) {}
		if (ImGui::Button("Apply"))
		{
			m_ActiveTool->Apply();
			m_bFinish = false; // hack
		}
		::ImGui::EndChild();
	}
}

//=================================================================================
bool C_ImageEditor::ToggleHistogram()
{
	CORE_LOG(E_Level::Error, E_Context::Core, "Histogram.");
	return false;
}

//=================================================================================
void C_ImageEditor::SetupToolPreview()
{
}

//=================================================================================
void C_ImageEditor::CreateTextures(Renderer::I_Renderer& renderer)
{
	using namespace Renderer;
	{
		const auto GPUSamplerHandle = renderer.GetRM().createSampler(SamplerDescriptor2D{
			.m_FilterMin = E_TextureFilter::Linear,
			.m_FilterMag = E_TextureFilter::Linear,
			.m_WrapS	 = E_WrapFunction::Repeat,
			.m_WrapT	 = E_WrapFunction::Repeat,
			.m_WrapU	 = E_WrapFunction::Repeat,
		});

		m_DeviceImage = renderer.GetRM().createTexture(TextureDescriptor{.name			= "Editor image",
																		 .width			= 1024,
																		 .height		= 1024,
																		 .type			= E_TextureType::TEXTURE_2D,
																		 .format		= E_TextureFormat::RGBA32f,
																		 .m_bStreamable = false});
		renderer.SetTextureSampler(m_DeviceImage, GPUSamplerHandle);
	}

	{
		auto GPUSamplerHandle = renderer.GetRM().createSampler(SamplerDescriptor2D{
			.m_FilterMin = E_TextureFilter::Nearest,
			.m_FilterMag = E_TextureFilter::Nearest,
			.m_WrapS	 = E_WrapFunction::Repeat,
			.m_WrapT	 = E_WrapFunction::Repeat,
			.m_WrapU	 = E_WrapFunction::Repeat,
		});
		m_Background		  = renderer.GetRM().createTexture(Renderer::TextureDescriptor{.name		  = "Transparent background",
																						   .width		  = s_BackgroundDim.x,
																						   .height		  = s_BackgroundDim.y,
																						   .type		  = Renderer::E_TextureType::TEXTURE_2D,
																						   .format		  = Renderer::E_TextureFormat::RGB32f,
																						   .m_bStreamable = false});
		renderer.SetTextureSampler(m_Background, GPUSamplerHandle);
	}


	m_Storage.SetAll(glm::vec4(Colours::white, 0.f));
	{
		static Renderer::C_TextureViewStorageCPU<float> storage(s_BackgroundDim.x, s_BackgroundDim.y, 3);
		storage.SetAll(glm::vec4(Colours::white, 0.f));
		Renderer::C_TextureView view(&storage);

		view.Set(glm::ivec2{0, 0}, glm::vec3{Colours::gray});
		view.Set(glm::ivec2{1, 1}, glm::vec3{Colours::gray});

		renderer.SetTextureData(m_Background, storage);
	}
}

} // namespace GLEngine::Editor