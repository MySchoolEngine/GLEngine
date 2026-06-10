#include <EditorStdafx.h>

#include <Editor/Editors/TrimeshPreview/TrimeshPreviewWindow.h>
#include <Editor/Editors/RayPreviewState.h>

#include <GUI/ImageViewer.h>

#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/RayCasting/Geometry/TrimeshModel.h>
#include <Renderer/RayCasting/Geometry/PrimitiveObject.h>
#include <Renderer/RayCasting/RayGeneration/InterleavedLinesFactory.h>
#include <Renderer/RayCasting/Light/RayAreaLight.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Colours.h>
#include <Renderer/Resources/ResourceManager.h>

#include <Physics/Primitives/AABB.h>

#include <Core/Application.h>

#include <imgui.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <thread>

namespace GLEngine::Editor {

//=================================================================================
C_TrimeshPreviewWindow::C_TrimeshPreviewWindow(GUID guid, GUI::C_GUIManager& guiMGR)
	: GUI::C_Window(guid, "Trimesh Preview")
	, m_GUIManager(guiMGR)
{
}

//=================================================================================
C_TrimeshPreviewWindow::~C_TrimeshPreviewWindow()
{
	for (auto& tab : m_TabbedView.m_Tabs)
		if (tab.m_Data)
			DestroyTabResources(tab);
}

//=================================================================================
void C_TrimeshPreviewWindow::OpenModel(Core::ResourceHandle<Renderer::C_TrimeshModel> handle)
{
	const auto path = handle.GetFilePath();
	if (m_TabbedView.TrySwitchTo([&](const S_TrimeshTab& t) {
			return t.m_Data && t.m_Data->m_Model.GetFilePath() == path;
		}))
		return;

	auto& tab	 = m_TabbedView.EmplaceTab();
	tab.m_Data	 = std::make_unique<S_TrimeshTabData>();
	tab.m_Data->m_Model = std::move(handle);
	tab.m_TabLabel		= tab.m_Data->m_Model.GetFilePath().filename().string();

	CreateRayPreviewState(tab.m_Data->m_Render, s_Resolution, "trimeshPreview",
						  Renderer::E_TextureFormat::RGB32f);
	SetupScene(*tab.m_Data);
	StartRender(*tab.m_Data);
}

//=================================================================================
void C_TrimeshPreviewWindow::SetupScene(S_TrimeshTabData& data)
{
	data.m_Scene.ClearScene();

	Physics::Primitives::S_AABB combinedAABB;
	for (const auto& trimesh : data.m_Model.GetResource().GetTrimeshes())
		combinedAABB.Add(trimesh.GetAABB());

	data.m_Scene.AddMesh(data.m_Model);

	const auto	  sphere = combinedAABB.GetSphere();
	const float	  r		 = sphere.m_radius;
	const glm::vec3 c	 = sphere.m_position;

	static const Renderer::MeshData::Material s_Black{.ambient			  = glm::vec4{},
													  .diffuse			  = glm::vec4{Colours::black, 0.f},
													  .specular			  = glm::vec4{},
													  .shininess		  = 0.f,
													  .textureIndex		  = -1,
													  .normalTextureIndex = -1,
													  .m_Name			  = "black"};

	const glm::vec3 lightNormal = glm::normalize(glm::vec3(0.f, -1.f, 0.f));
	auto disc = Physics::Primitives::S_Disc(lightNormal, c + glm::vec3(0.f, r * 3.f, 0.f), r * 1.5f);
	disc.plane.twoSided = false;
	auto discPrimitive	= std::make_shared<Renderer::C_Primitive<Physics::Primitives::S_Disc>>(disc);
	discPrimitive->SetMaterial(data.m_Scene.AddMaterial(s_Black).get());
	data.m_Scene.AddLight(std::make_shared<Renderer::RayTracing::C_AreaLight>(
		glm::vec3(1.f, 1.f, 1.f) * 5.f, discPrimitive));

	SetupCamera(data);

	data.m_Render.m_Renderer = std::make_unique<Renderer::C_RayRenderer>(data.m_Scene);
}

//=================================================================================
void C_TrimeshPreviewWindow::SetupCamera(S_TrimeshTabData& data)
{
	Physics::Primitives::S_AABB combinedAABB;
	for (const auto& trimesh : data.m_Model.GetResource().GetTrimeshes())
		combinedAABB.Add(trimesh.GetAABB());

	const auto	  sphere   = combinedAABB.GetSphere();
	const float	  r		   = sphere.m_radius > 0.f ? sphere.m_radius : 1.f;
	const glm::vec3 center = sphere.m_position;

	const float distance = r * 3.f;
	const float nearZ	 = std::max(0.01f, distance - r);
	const float farZ	 = distance + r;
	const float fovY	 = glm::degrees(2.f * std::atan((r * 1.1f) / distance));

	data.m_Camera.SetupCameraView(distance, center, 45.f, 35.f);
	data.m_Camera.SetupCameraProjection(nearZ, farZ, 1.f, fovY);
	data.m_Camera.Update();
}

//=================================================================================
void C_TrimeshPreviewWindow::StartRender(S_TrimeshTabData& data)
{
	if (data.m_Render.m_Running.load())
		return;

	data.m_Render.m_NumSamples.store(0);
	data.m_Render.m_StopRequested.store(false);
	data.m_Render.m_Running.store(true);

	std::thread([&data]() {
		while (!data.m_Render.m_StopRequested.load())
		{
			const int samplesBefore = data.m_Render.m_NumSamples.load();
			if (samplesBefore >= s_TargetSamples)
				break;

			data.m_Render.m_Renderer->Render(data.m_Camera,
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
void C_TrimeshPreviewWindow::DestroyTabResources(S_TrimeshTab& tab)
{
	if (!tab.m_Data)
		return;
	DestroyRayPreviewState(tab.m_Data->m_Render);
}

//=================================================================================
void C_TrimeshPreviewWindow::Update()
{
	for (auto& tab : m_TabbedView.m_Tabs)
		if (tab.m_Data)
			UploadPreviewStorage(tab.m_Data->m_Render);
}

//=================================================================================
void C_TrimeshPreviewWindow::DrawComponents() const
{
	if (!m_TabbedView.HasTabs())
	{
		ImGui::TextDisabled("No model open. Double-click a .tri file in the Resource Manager.");
		return;
	}

	m_TabbedView.Draw(
		"##TrimeshTabs",
		[this](const S_TrimeshTab& tab) { DrawTabContent(tab); },
		[this](S_TrimeshTab& tab) { const_cast<C_TrimeshPreviewWindow*>(this)->DestroyTabResources(tab); });
}

//=================================================================================
void C_TrimeshPreviewWindow::DrawTabContent(const S_TrimeshTab& tab) const
{
	if (!tab.m_Data)
		return;

	const S_TrimeshTabData& data = *tab.m_Data;

	if (data.m_Render.m_GUIImage)
		std::ignore = data.m_Render.m_GUIImage->Draw();

	if (DrawRenderProgress(data.m_Render, s_TargetSamples))
		const_cast<C_TrimeshPreviewWindow*>(this)->StartRender(*tab.m_Data);
}

//=================================================================================
void C_TrimeshPreviewWindow::RequestDestroy()
{
	for (auto& tab : m_TabbedView.m_Tabs)
		if (tab.m_Data)
			tab.m_Data->m_Render.m_StopRequested.store(true);
	m_WantToBeDestroyed = true;
}

//=================================================================================
bool C_TrimeshPreviewWindow::CanDestroy() const
{
	return std::none_of(m_TabbedView.m_Tabs.begin(), m_TabbedView.m_Tabs.end(),
						[](const S_TrimeshTab& t) { return t.m_Data && t.m_Data->m_Render.m_Running.load(); });
}

} // namespace GLEngine::Editor
