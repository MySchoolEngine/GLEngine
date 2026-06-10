#include <EditorStdafx.h>

#include <Editor/Editors/MaterialPreview/MaterialPreviewWindow.h>
#include <Editor/Editors/RayPreviewState.h>

#include <Renderer/Colours.h>
#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/Materials/PBRMaterialData.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/Geometry/PrimitiveObject.h>
#include <Renderer/RayCasting/Light/RayAreaLight.h>
#include <Renderer/RayCasting/RayGeneration/InterleavedLinesFactory.h>
#include <Renderer/Resources/ResourceManager.h>
#include <Renderer/Textures/TextureView.h>

#include <GUI/GUIManager.h>
#include <GUI/ImageViewer.h>
#include <GUI/ResourceDialogWindow.h>

#include <Physics/Primitives/Disc.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Sphere.h>

#include <Core/Application.h>

#include "../../../../vendor/Assimp/code/AssetLib/M3D/m3d.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <imgui.h>
#include <thread>

namespace GLEngine::Editor {

//=================================================================================
C_MaterialPreviewWindow::C_MaterialPreviewWindow(GUID guid, GUI::C_GUIManager& guiMGR)
	: GUI::C_Window(guid, "Material Preview")
	, m_FileMenu("File")
	, m_GUIManager(guiMGR)
{
	m_FileMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("New material", [this]() {
		NewMaterial();
		return true;
	}));
	m_FileMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Save", [this]() {
		if (m_TabbedView.HasTabs() && m_TabbedView.ActiveTab().m_Data)
			SaveMaterial(*m_TabbedView.ActiveTab().m_Data);
		return true;
	}));
	m_FileMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Save As...", [this]() {
		if (m_TabbedView.HasTabs() && m_TabbedView.ActiveTab().m_Data)
			SaveMaterialAs(*m_TabbedView.ActiveTab().m_Data);
		return true;
	}));

	AddMenu(m_FileMenu);

	SetupCamera();
}

//=================================================================================
C_MaterialPreviewWindow::~C_MaterialPreviewWindow()
{
	for (auto& tab : m_TabbedView.m_Tabs)
		if (tab.m_Data)
			DestroyTabResources(tab);
}

//=================================================================================
void C_MaterialPreviewWindow::OpenMaterial(Core::ResourceHandle<Renderer::MaterialResource> handle)
{
	if (m_TabbedView.TrySwitchTo([&](const S_MaterialTab& t) {
			return t.m_Data && t.m_Data->m_Material == handle;
		}))
		return;

	auto& tab		   = m_TabbedView.EmplaceTab();
	tab.m_Data		   = std::make_unique<S_MaterialTabData>();
	tab.m_Data->m_Material = std::move(handle);
	tab.m_TabLabel	   = tab.m_Data->m_Material.GetFilePath().filename().string();

	CreateRayPreviewState(tab.m_Data->m_Render, s_Resolution, "materialPreview",
						  Renderer::E_TextureFormat::RGB32f);
	SetupScene(*tab.m_Data);
	StartRender(*tab.m_Data);
}

//=================================================================================
void C_MaterialPreviewWindow::SetupScene(S_MaterialTabData& data)
{
	data.m_Scene.ClearScene();

	if (!data.m_Material.IsReady())
		return;

	auto&		matInterface = data.m_Scene.AddMaterial(data.m_Material);
	const auto* matPBR		 = dynamic_cast<const Renderer::C_PBRMaterialData*>(data.m_Material.GetResource().GetMaterialData());

	std::shared_ptr<Renderer::I_RayGeometryObject> prim;
	if (data.m_PreviewShape == E_PreviewShape::Sphere)
		prim = std::make_shared<Renderer::C_Primitive<Physics::Primitives::S_Sphere>>(Physics::Primitives::S_Sphere{glm::vec3(0.f), 1.f});
	else
		prim = std::make_shared<Renderer::C_Primitive<Physics::Primitives::S_Plane>>(Physics::Primitives::S_Plane{glm::vec3(0.f, 1.f, 0.f), 0.f});

	prim->SetMaterial(matInterface.get());
	if (matPBR)
		prim->SetAlphaMask(matPBR->GetColorMapRes());
	data.m_Scene.AddObject(std::move(prim));

	static const Renderer::MeshData::Material s_Black{.ambient			  = glm::vec4{},
													  .diffuse			  = glm::vec4{Colours::black, 0.f},
													  .specular			  = glm::vec4{},
													  .shininess		  = 0.f,
													  .textureIndex		  = -1,
													  .normalTextureIndex = -1,
													  .m_Name			  = "black"};

	const glm::vec3 lightNormal = glm::normalize(glm::vec3(0.f, -1.f, 0.f));
	auto			disc		= Physics::Primitives::S_Disc(lightNormal, glm::vec3(0.f, 4.f, 0.f), 2.f);
	disc.plane.twoSided			= false;
	auto discPrimitive			= std::make_shared<Renderer::C_Primitive<Physics::Primitives::S_Disc>>(disc);
	discPrimitive->SetMaterial(data.m_Scene.AddMaterial(s_Black).get());
	data.m_Scene.AddLight(std::make_shared<Renderer::RayTracing::C_AreaLight>(glm::vec3(1.f, 1.f, 1.f), discPrimitive));

	data.m_Render.m_Renderer = std::make_unique<Renderer::C_RayRenderer>(data.m_Scene);
}

//=================================================================================
void C_MaterialPreviewWindow::SetupCamera()
{
	m_Camera.SetupCameraView(3.f, glm::vec3(0.f), 45.f, 35.f);
	m_Camera.SetupCameraProjection(2.f, 4.f, 1.f, 40.f);
	m_Camera.Update();
}

//=================================================================================
void C_MaterialPreviewWindow::Update()
{
	for (auto& tab : m_TabbedView.m_Tabs)
	{
		if (!tab.m_Data)
			continue;
		auto& data = *tab.m_Data;
		if (data.m_RebuildPending.exchange(false))
			RebuildAndRestart(data);
		UploadPreviewStorage(data.m_Render);
	}
}

//=================================================================================
void C_MaterialPreviewWindow::DrawComponents() const
{
	if (!m_TabbedView.HasTabs())
	{
		ImGui::TextUnformatted("No materials open. Use File > New material or double-click a .glmat in the Resource Manager.");
		return;
	}

	m_TabbedView.Draw(
		"##MaterialTabs",
		[this](S_MaterialTab& tab) { DrawTabContent(tab); },
		[this](S_MaterialTab& tab) {
			if (tab.m_Data)
				const_cast<C_MaterialPreviewWindow*>(this)->SaveMaterial(*tab.m_Data);
		},
		[this](S_MaterialTab& tab) { const_cast<C_MaterialPreviewWindow*>(this)->DestroyTabResources(tab); });

	m_TabbedView.DrawEditorCloseModals(
		"##MatClose",
		[this](S_MaterialTab& tab) {
			if (tab.m_Data)
				const_cast<C_MaterialPreviewWindow*>(this)->SaveMaterial(*tab.m_Data);
		},
		[this] { const_cast<C_MaterialPreviewWindow*>(this)->GUI::C_Window::OnHide(); });
}

//=================================================================================
void C_MaterialPreviewWindow::DrawTabContent(const S_MaterialTab& tab) const
{
	auto& data = *tab.m_Data;
	auto* self = const_cast<C_MaterialPreviewWindow*>(this);

	if (data.m_Material.IsReady() && data.m_Material.GetResource().IsModified())
		ImGui::Text("Material modified");

	if (data.m_Render.m_GUIImage)
		std::ignore = data.m_Render.m_GUIImage->Draw();

	// Shape selector
	ImGui::Text("Shape:");
	ImGui::SameLine();
	if (ImGui::RadioButton("Sphere", data.m_PreviewShape == E_PreviewShape::Sphere) && data.m_PreviewShape != E_PreviewShape::Sphere)
	{
		const_cast<S_MaterialTabData&>(data).m_PreviewShape = E_PreviewShape::Sphere;
		const_cast<S_MaterialTabData&>(data).m_RebuildPending.store(true);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Plane", data.m_PreviewShape == E_PreviewShape::Plane) && data.m_PreviewShape != E_PreviewShape::Plane)
	{
		const_cast<S_MaterialTabData&>(data).m_PreviewShape = E_PreviewShape::Plane;
		const_cast<S_MaterialTabData&>(data).m_RebuildPending.store(true);
	}

	// Render mode selector (GPU disabled until implemented)
	ImGui::SameLine();
	ImGui::Spacing();
	ImGui::SameLine();
	ImGui::Text("Mode:");
	ImGui::SameLine();
	ImGui::RadioButton("CPU", true);
	ImGui::SameLine();
	ImGui::BeginDisabled();
	ImGui::RadioButton("GPU", false);
	ImGui::EndDisabled();

	ImGui::Separator();

	// Material properties
	if (data.m_Material.IsReady())
	{
		if (const_cast<S_MaterialTabData&>(data).m_Material.GetResource().DrawGUI())
		{
			self->RebuildAndRestart(const_cast<S_MaterialTabData&>(data));
			const_cast<S_MaterialTab&>(tab).m_bModified = true;
		}
	}

	ImGui::Separator();

	if (DrawRenderProgress(data.m_Render, s_TargetSamples))
		self->StartRender(const_cast<S_MaterialTabData&>(data));
}

//=================================================================================
void C_MaterialPreviewWindow::RebuildAndRestart(S_MaterialTabData& data)
{
	StopPreviewRender(data.m_Render);
	data.m_Render.m_StopRequested.store(false);

	constexpr glm::vec4 black{0.f, 0.f, 0.f, 1.f};
	Renderer::C_TextureView(&*data.m_Render.m_ImageStorage).ClearColor(black);
	Renderer::C_TextureView(&*data.m_Render.m_SamplesStorage).ClearColor(black);

	SetupScene(data);
	StartRender(data);
}

//=================================================================================
void C_MaterialPreviewWindow::DestroyTabResources(S_MaterialTab& tab)
{
	if (!tab.m_Data)
		return;
	DestroyRayPreviewState(tab.m_Data->m_Render);
}

//=================================================================================
void C_MaterialPreviewWindow::NewMaterial()
{
	const auto dialogGUID = NextGUID();
	auto*	   dialog	  = new GUI::C_FileDialogWindow(
		 ".glmat", "Save material as...",
		 [this, dialogGUID](const std::filesystem::path& savePath, GUI::C_GUIManager& guiMgr) {
			 auto& rm		  = Core::C_ResourceManager::Instance();
			 auto  newResource = rm.CreateNewResource<Renderer::MaterialResource>(savePath);
			 if (newResource.IsReady())
			 {
				 newResource.GetResource().SetMaterialData(std::make_shared<Renderer::C_PBRMaterialData>());
				 OpenMaterial(std::move(newResource));
			 }
			 guiMgr.DestroyWindow(dialogGUID);
		 },
		 dialogGUID, Renderer::TextureResource::GetResourceDataPath());
	m_GUIManager.AddCustomWindow(dialog);
	dialog->SetVisible();
}

//=================================================================================
void C_MaterialPreviewWindow::SaveMaterial(S_MaterialTabData& data)
{
	if (!data.m_Material.IsReady())
		return;
	(void)data.m_Material.GetResource().Save();
	for (auto& tab : m_TabbedView.m_Tabs)
		if (tab.m_Data.get() == &data)
			tab.m_bModified = false;
}

//=================================================================================
void C_MaterialPreviewWindow::SaveMaterialAs(S_MaterialTabData& data)
{
	if (!data.m_Material.IsReady())
		return;

	const auto dialogGUID = NextGUID();
	auto*	   dialog	  = new GUI::C_FileDialogWindow(
		 ".glmat", "Save material as...",
		 [&data, dialogGUID](const std::filesystem::path& /*savePath*/, GUI::C_GUIManager& guiMgr) {
			 // TODO: implement copy-to-new-path
			 guiMgr.DestroyWindow(dialogGUID);
		 },
		 dialogGUID, Renderer::TextureResource::GetResourceDataPath());
	m_GUIManager.AddCustomWindow(dialog);
	dialog->SetVisible();
}

//=================================================================================
void C_MaterialPreviewWindow::StartRender(S_MaterialTabData& data)
{
	if (data.m_Render.m_Running.load())
		return;

	data.m_Render.m_NumSamples.store(0);
	data.m_Render.m_StopRequested.store(false);
	data.m_Render.m_Running.store(true);

	std::thread([&data, this]() {
		while (!data.m_Render.m_StopRequested.load())
		{
			const int samplesBefore = data.m_Render.m_NumSamples.load();
			if (samplesBefore >= s_TargetSamples)
				break;

			data.m_Render.m_Renderer->Render(m_Camera,
											 *data.m_Render.m_ImageStorage,
											 *data.m_Render.m_SamplesStorage,
											 &data.m_Render.m_ImageLock,
											 samplesBefore,
											 Renderer::C_InterleavedLinesFactory{4});
			data.m_Render.m_NumSamples.fetch_add(1);
		}
		data.m_Render.m_Running.store(false);
	}).detach();
}

//=================================================================================
void C_MaterialPreviewWindow::RequestDestroy()
{
	for (auto& tab : m_TabbedView.m_Tabs)
		if (tab.m_Data)
			tab.m_Data->m_Render.m_StopRequested.store(true);
	m_WantToBeDestroyed = true;
}

//=================================================================================
void C_MaterialPreviewWindow::OnHide()
{
	const bool anyModified = std::any_of(m_TabbedView.m_Tabs.begin(), m_TabbedView.m_Tabs.end(), [](const S_MaterialTab& t) { return t.m_bModified; });
	if (anyModified)
	{
		m_TabbedView.RequestEditorClose();
		return;
	}
	GUI::C_Window::OnHide();
}

//=================================================================================
bool C_MaterialPreviewWindow::CanDestroy() const
{
	return std::none_of(m_TabbedView.m_Tabs.begin(), m_TabbedView.m_Tabs.end(), [](const S_MaterialTab& t) { return t.m_Data && t.m_Data->m_Render.m_Running.load(); });
}

} // namespace GLEngine::Editor
