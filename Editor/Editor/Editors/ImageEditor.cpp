#include <EditorStdafx.h>

#include <Editor/Editors/Image/Tools/BrickGenerator.h>
#include <Editor/Editors/Image/Tools/PerlinNoiseGenerator.h>
#include <Editor/Editors/Image/Tools/WaveGenerator.h>
#include <Editor/Editors/ImageEditor.h>
#include <Editor/Editors/ImageEditorTool.h>

#include <Renderer/Colours.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Render/CPUCompute.h>
#include <Renderer/Render/CPURasterizer.h>
#include <Renderer/Resources/ResourceManager.h>
#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureResource.h>
#include <Renderer/Textures/TextureView.h>

#include <GUI/FileDialogWindow.h>
#include <GUI/GUIManager.h>
#include <GUI/Menu/Menu.h>
#include <GUI/Menu/MenuItem.h>
#include <GUI/ReflectionGUI.h>

#include <Core/Application.h>
#include <Core/EventSystem/Event/ResourceEvents.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/HighResolutionTimer.h>

#include <imgui_internal.h>

namespace GLEngine::Editor {
static constexpr glm::uvec2 s_BackgroundDim{2, 2};

//=================================================================================
C_ImageEditor::C_ImageEditor(GUID guid, GUI::C_GUIManager& guiMGR, T_EventCallback eventCallback, Core::ResourceHandle<Renderer::TextureResource> initialResource)
	: GUI::C_Window(guid, "Image editor")
	, m_FileMenu("File")
	, m_ToolsdMenu("Tools")
	, m_GUIManager(guiMGR)
	, m_EventCallback(std::move(eventCallback))
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();

	// Create shared checkerboard background texture
	{
		auto GPUSamplerHandle = renderer.GetRM().createSampler(Renderer::SamplerDescriptor2D{
			.m_FilterMin = Renderer::E_TextureFilter::Nearest,
			.m_FilterMag = Renderer::E_TextureFilter::Nearest,
			.m_WrapS	 = Renderer::E_WrapFunction::Repeat,
			.m_WrapT	 = Renderer::E_WrapFunction::Repeat,
			.m_WrapU	 = Renderer::E_WrapFunction::Repeat,
		});
		m_Background		  = renderer.GetRM().createTexture(Renderer::TextureDescriptor{.name		  = "Transparent background",
																				   .width		  = s_BackgroundDim.x,
																				   .height		  = s_BackgroundDim.y,
																				   .type		  = Renderer::E_TextureType::TEXTURE_2D,
																				   .format		  = Renderer::E_TextureFormat::RGB32f,
																				   .m_bStreamable = false});
		renderer.SetTextureSampler(m_Background, GPUSamplerHandle);

		// TODO is static enough to solve it?
		static Renderer::C_TextureViewStorageCPU<float> bgStorage(s_BackgroundDim.x, s_BackgroundDim.y, 3);
		bgStorage.SetAll(glm::vec4(Colours::white, 0.f));
		Renderer::C_TextureView bgView(&bgStorage);
		bgView.Set(glm::ivec2{0, 0}, glm::vec3{Colours::gray});
		bgView.Set(glm::ivec2{1, 1}, glm::vec3{Colours::gray});
		renderer.SetTextureData(m_Background, bgStorage);
	}

	// === File Menu ===
	m_FileMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("New Image", [this]() {
		NewImage();
		return true;
	}));
	m_FileMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Save", [this]() {
		if (!m_Tabs.empty())
			SaveTab(ActiveTab());
		return true;
	}));
	m_FileMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Save As...", [this]() {
		if (!m_Tabs.empty())
			SaveTabAs(ActiveTab());
		return true;
	}));

	// === Playground Menu ===
	m_ToolsdMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Brick", [this]() {
		if (m_Tabs.empty())
			return false;
		auto& tab		 = ActiveTab();
		tab.m_ActiveTool = std::make_unique<C_BrickGenerator>(Renderer::C_TextureView(&*tab.m_Storage));
		return true;
	}));
	m_ToolsdMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Blur", [this]() {
		if (m_Tabs.empty())
			return false;
		auto& tab		 = ActiveTab();
		tab.m_ActiveTool = std::make_unique<C_GaussianBlur>(Renderer::C_TextureView(&*tab.m_Storage));
		return true;
	}));
	m_ToolsdMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Wave", [this]() {
		if (m_Tabs.empty())
			return false;
		auto& tab		 = ActiveTab();
		tab.m_ActiveTool = std::make_unique<C_WaveGenerator>(Renderer::C_TextureView(&*tab.m_Storage));
		return true;
	}));
	m_ToolsdMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Perlin Noise", [this]() {
		if (m_Tabs.empty())
			return false;
		auto& tab		 = ActiveTab();
		tab.m_ActiveTool = std::make_unique<C_PerlinNoise>(Renderer::C_TextureView(&*tab.m_Storage));
		return true;
	}));

	AddMenu(m_FileMenu);
	AddMenu(m_ToolsdMenu);

	if (initialResource.IsReady())
		OpenImage(initialResource);
	// If no initial resource the window shows the empty state message in DrawComponents
}

//=================================================================================
C_ImageEditor::~C_ImageEditor()
{
	for (auto& tab : m_Tabs)
	{
		DestroyTabResources(tab);
	}
	auto& rm = Core::C_Application::Get().GetActiveRenderer().GetRM();
	if (m_Background.IsValid())
		rm.destoryTexture(m_Background);
}

//=================================================================================
C_ImageEditor::S_ImageTab& C_ImageEditor::ActiveTab()
{
	return m_Tabs[m_ActiveTabIndex];
}

//=================================================================================
const C_ImageEditor::S_ImageTab& C_ImageEditor::ActiveTab() const
{
	return m_Tabs[m_ActiveTabIndex];
}

//=================================================================================
C_ImageEditor::S_ImageTab& C_ImageEditor::CreateTab(Core::ResourceHandle<Renderer::TextureResource> handle, std::uint32_t width, std::uint32_t height)
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();

	auto& tab			 = m_Tabs.emplace_back();
	tab.m_ResourceHandle = handle;

	// Create per-tab GPU texture
	const auto samplerHandle = renderer.GetRM().createSampler(Renderer::SamplerDescriptor2D{
		.m_FilterMin = Renderer::E_TextureFilter::Linear,
		.m_FilterMag = Renderer::E_TextureFilter::Linear,
		.m_WrapS	 = Renderer::E_WrapFunction::Repeat,
		.m_WrapT	 = Renderer::E_WrapFunction::Repeat,
		.m_WrapU	 = Renderer::E_WrapFunction::Repeat,
	});
	tab.m_DeviceImage		 = renderer.GetRM().createTexture(Renderer::TextureDescriptor{.name			 = "Editor image",
																					  .width		 = width,
																					  .height		 = height,
																					  .type			 = Renderer::E_TextureType::TEXTURE_2D,
																					  .format		 = Renderer::E_TextureFormat::RGBA32f,
																					  .m_bStreamable = false});
	renderer.SetTextureSampler(tab.m_DeviceImage, samplerHandle);

	// Create per-tab ImageViewer
	tab.m_GUIImage.emplace(tab.m_DeviceImage);
	tab.m_GUIImage->SetSize({800, 800});
	tab.m_GUIImage->SetBackground(m_Background, {1.f / (s_BackgroundDim.x * 15.f), 1.f / (s_BackgroundDim.x * 15.f)});

	return tab;
}

//=================================================================================
void C_ImageEditor::OpenImage(Core::ResourceHandle<Renderer::TextureResource> handle)
{
	// Deduplication: if already open, just switch focus
	for (int i = 0; i < static_cast<int>(m_Tabs.size()); ++i)
	{
		if (m_Tabs[i].m_ResourceHandle == handle)
		{
			m_ActiveTabIndex = i;
			return;
		}
	}

	if (!handle.IsReady())
	{
		// Resource not yet ready — nothing to show yet
		// TODO: handle async loading by polling in Update()
		return;
	}

	const auto& srcStorage = handle.GetResource().GetStorage();
	const auto	dim		   = srcStorage.GetDimensions();
	const auto	nChannels  = srcStorage.GetNumElements();

	auto& tab = CreateTab(handle, dim.x, dim.y);

	// TODO make clone that would be faster
	// Pixel-by-pixel copy into float storage (handles any source format)
	tab.m_Storage.emplace(dim.x, dim.y, nChannels);
	const std::size_t numPixels = static_cast<std::size_t>(dim.x) * dim.y;
	for (std::size_t i = 0; i < numPixels; ++i)
		tab.m_Storage->SetPixel(srcStorage.GetPixel(i), i);

	tab.m_TabLabel	 = handle.GetFilePath().filename().string();
	tab.m_bNeedsSync = true;
}

//=================================================================================
void C_ImageEditor::NewImage(std::uint32_t width, std::uint32_t height)
{
	auto& tab = CreateTab({}, width, height);
	tab.m_Storage.emplace(width, height, 4);
	tab.m_Storage->SetAll(glm::vec4(Colours::white, 0.f));
	tab.m_TabLabel	 = "New Image";
	tab.m_bModified	 = false;
	tab.m_bNeedsSync = true;

	// Playground: draw test circles on the blank canvas
	std::thread([&tab]() {
		::Utils::HighResolutionTimer timer;
		timer.reset();
		Renderer::C_TextureView view(&*tab.m_Storage);
		view.EnableBlending();
		constexpr glm::uvec2		  center{512, 512};
		Renderer::C_CPURasterizer rasterizer(view);
		rasterizer.DrawCircle(Colours::red, center, 200);
		rasterizer.DrawCircle(Colours::red, center, 400);
		tab.m_bNeedsSync = true;
		CORE_LOG(E_Level::Info, E_Context::Render, "Circle render time {}s", static_cast<float>(timer.getElapsedTimeFromLastQueryMilliseconds()) / 1000.f);
	}).detach();
}

//=================================================================================
void C_ImageEditor::DestroyTabResources(S_ImageTab& tab) const
{
	auto& rm = Core::C_Application::Get().GetActiveRenderer().GetRM();
	if (tab.m_DeviceImage.IsValid())
		rm.destoryTexture(tab.m_DeviceImage);
}

//=================================================================================
void C_ImageEditor::SyncTabToGPU(const S_ImageTab& tab) const
{
	if (!tab.m_Storage)
		return;
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	renderer.SetTextureData(tab.m_DeviceImage, *tab.m_Storage);
}

//=================================================================================
void C_ImageEditor::SaveTab(S_ImageTab& tab)
{
	if (!tab.m_ResourceHandle)
	{
		SaveTabAs(tab);
		return;
	}
	if (!tab.m_Storage)
		return;

	Renderer::Textures::TextureLoader tl;
	if (tl.SaveTexture(tab.m_ResourceHandle.GetFilePath(), &*tab.m_Storage))
	{
		tab.m_bModified = false;
		if (m_EventCallback)
		{
			Core::ResourceModifiedEvent<Renderer::TextureResource> evt(tab.m_ResourceHandle);
			m_EventCallback(evt);
		}
	}
}

//=================================================================================
void C_ImageEditor::SaveTabAs(S_ImageTab& tab)
{
	if (!tab.m_Storage)
		return;

	const auto dialogGUID = NextGUID();
	auto*	   dialog	  = new GUI::C_FileDialogWindow(
		 ".bmp,.hdr,.ppm", "Save image as...",
		 [this, &tab, dialogGUID](const std::filesystem::path& savePath, GUI::C_GUIManager& guiMgr) {
			 Renderer::Textures::TextureLoader tl;
			 if (tl.SaveTexture(savePath, &*tab.m_Storage))
			 {
				 // Register with ResourceManager and update handle
				 tab.m_ResourceHandle = Core::C_ResourceManager::Instance().LoadResource<Renderer::TextureResource>(savePath, true);
				 tab.m_TabLabel		  = savePath.filename().string();
				 tab.m_bModified	  = false;

				 if (m_EventCallback)
				 {
					 Core::ResourceCreatedEvent<Renderer::TextureResource> evt(tab.m_ResourceHandle);
					 m_EventCallback(evt);
				 }
			 }
			 guiMgr.DestroyWindow(dialogGUID);
		 },
		 dialogGUID, "./Images");
	m_GUIManager.AddCustomWindow(dialog);
	dialog->SetVisible();
}

//=================================================================================
bool C_ImageEditor::CanDestroy() const
{
	// Block destruction while any tab has unsaved changes
	// TODO: show "Save before closing?" dialog
	return std::none_of(m_Tabs.begin(), m_Tabs.end(), [](const S_ImageTab& t) { return t.m_bModified; });
}

//=================================================================================
void C_ImageEditor::Update()
{
	for (auto& tab : m_Tabs)
	{
		if (!tab.m_bNeedsSync)
			continue;
		SyncTabToGPU(tab);
		tab.m_bNeedsSync = false;

		if (&tab == &ActiveTab() && tab.m_Storage)
		{
			Renderer::C_CPUCompute compute;
			compute.ComputeHistogram(Renderer::C_TextureView(&*tab.m_Storage), Renderer::C_TextureView(&tab.m_Histogram));
		}
	}
}

//=================================================================================
void C_ImageEditor::DrawComponents() const
{
	if (m_Tabs.empty())
	{
		ImGui::TextUnformatted("No images open. Use File > New Image or File > Save As.");
		return;
	}

	if (ImGui::BeginTabBar("##ImageTabs", ImGuiTabBarFlags_AutoSelectNewTabs))
	{
		for (int i = 0; i < static_cast<int>(m_Tabs.size()); ++i)
		{
			auto& tab = m_Tabs[i];

			const std::string label = tab.m_TabLabel + (tab.m_bModified ? " *" : "") + "##tab" + std::to_string(i);
			bool			  open	= true;

			if (ImGui::BeginTabItem(label.c_str(), &open))
			{
				// TODO if we switch tabs, what about tools?
				m_ActiveTabIndex = i;
				DrawTabContent(tab);
				ImGui::EndTabItem();
			}

			if (!open)
			{
				DestroyTabResources(tab);
				m_Tabs.erase(m_Tabs.begin() + i);
				if (m_ActiveTabIndex >= static_cast<int>(m_Tabs.size()))
					m_ActiveTabIndex = std::max(0, static_cast<int>(m_Tabs.size()) - 1);
				--i;
			}
		}
		ImGui::EndTabBar();
	}
}

//=================================================================================
void C_ImageEditor::DrawTabContent(const S_ImageTab& tab) const
{
	if (tab.m_GUIImage)
		tab.m_GUIImage->Draw();

	ImGui::SameLine();
	if (ImGui::BeginChild("##ImageTools", ImVec2(0, 0), false))
	{
		ImGui::PlotHistogram("Luminance histogram", static_cast<const float*>(tab.m_Histogram.GetData()), 255, 0, nullptr, std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::max(), ImVec2{0, 50});

		if (tab.m_ActiveTool)
		{
			rttr::instance obj(tab.m_ActiveTool.get());
			if (!GUI::DrawAllPropertyGUI(obj).empty()) {}
			if (ImGui::Button("Apply"))
			{
				tab.m_ActiveTool->Apply();
				const_cast<S_ImageTab&>(tab).m_bNeedsSync = true;
				const_cast<S_ImageTab&>(tab).m_bModified  = true;
			}
		}
	}
	ImGui::EndChild();
}

} // namespace GLEngine::Editor
