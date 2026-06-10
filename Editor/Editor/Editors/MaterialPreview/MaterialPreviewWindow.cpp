#include <EditorStdafx.h>

#include <Editor/Editors/MaterialPreview/MaterialPreviewWindow.h>

#include <Renderer/Colours.h>
#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/Materials/PBRMaterialData.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/Geometry/PrimitiveObject.h>
#include <Renderer/RayCasting/RayGeneration/InterleavedLinesFactory.h>
#include <Renderer/RayCasting/Light/RayAreaLight.h>
#include <Renderer/Resources/ResourceManager.h>
#include <Renderer/Textures/TextureView.h>

#include <GUI/GUIManager.h>
#include <GUI/ResourceDialogWindow.h>

#include <Physics/Primitives/Disc.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Sphere.h>

#include <Core/Application.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <imgui.h>
#include <thread>

namespace GLEngine::Editor {

//=================================================================================
C_MaterialPreviewWindow::C_MaterialPreviewWindow(GUID guid, GUI::C_GUIManager& guiMGR, Core::ResourceHandle<Renderer::MaterialResource> material)
	: GUI::C_Window(guid, "Material Preview")
	, m_Material(std::move(material))
	, m_ImageStorage(s_Resolution.x, s_Resolution.y, 3)
	, m_SamplesStorage(s_Resolution.x, s_Resolution.y, 3)
	, m_GUIImage({})
	, m_FileMenu("File")
	, m_GUIManager(guiMGR)
{
	// === File Menu ===
	m_FileMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("New material", [this]() {
		NewMaterial();
		return true;
	}));
	m_FileMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Save", [this]() {
		SaveMaterial();
		return true;
	}));
	m_FileMenu.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Save As...", [this]() {
		SaveMaterialAs();
		return true;
	}));

	auto& renderer = Core::C_Application::Get().GetActiveRenderer();

	m_GPUImageHandle		 = renderer.GetRM().createTexture(Renderer::TextureDescriptor{
		.name		   = "materialPreview",
		.width		   = s_Resolution.x,
		.height		   = s_Resolution.y,
		.type		   = Renderer::E_TextureType::TEXTURE_2D,
		.format		   = Renderer::E_TextureFormat::RGB32f,
		.m_bStreamable = false,
	});
	const auto samplerHandle = renderer.GetRM().createSampler(Renderer::SamplerDescriptor2D{
		.m_FilterMin = Renderer::E_TextureFilter::Linear,
		.m_FilterMag = Renderer::E_TextureFilter::Linear,
		.m_WrapS	 = Renderer::E_WrapFunction::Repeat,
		.m_WrapT	 = Renderer::E_WrapFunction::Repeat,
		.m_WrapU	 = Renderer::E_WrapFunction::Repeat,
	});
	renderer.SetTextureSampler(m_GPUImageHandle, samplerHandle);

	m_GUIImage = GUI::C_ImageViewer(m_GPUImageHandle);
	m_GUIImage.SetSize({s_Resolution.x, s_Resolution.y});

	constexpr glm::vec4 black{0.f, 0.f, 0.f, 1.f};
	Renderer::C_TextureView(&m_ImageStorage).ClearColor(black);
	Renderer::C_TextureView(&m_SamplesStorage).ClearColor(black);
	renderer.SetTextureData(m_GPUImageHandle, m_ImageStorage);

	SetupScene();
	StartRender();
}

//=================================================================================
void C_MaterialPreviewWindow::SetupScene()
{
	m_Scene.ClearScene();

	auto&		matInterface = m_Scene.AddMaterial(m_Material);
	const auto* matPBR		 = dynamic_cast<const Renderer::C_PBRMaterialData*>(m_Material.GetResource().GetMaterialData());

	std::shared_ptr<Renderer::I_RayGeometryObject> prim;

	if (m_PreviewShape == E_PreviewShape::Sphere)
	{
		prim = std::make_shared<Renderer::C_Primitive<Physics::Primitives::S_Sphere>>(Physics::Primitives::S_Sphere{glm::vec3(0.f), 1.f});
	}
	else // Plane
	{
		prim = std::make_shared<Renderer::C_Primitive<Physics::Primitives::S_Plane>>(Physics::Primitives::S_Plane{glm::vec3(0.f, 1.f, 0.f), 0.f});
	}
	prim->SetMaterial(matInterface.get());
	if (matPBR)
	{
		prim->SetAlphaMask(matPBR->GetColorMapRes());
	}
	m_Scene.AddObject(std::move(prim));

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
	discPrimitive->SetMaterial(m_Scene.AddMaterial(s_Black).get());
	m_Scene.AddLight(std::make_shared<Renderer::RayTracing::C_AreaLight>(glm::vec3(1.f, 1.f, 1.f), discPrimitive));

	SetupCamera();

	m_Renderer = std::make_unique<Renderer::C_RayRenderer>(m_Scene);
}

//=================================================================================
void C_MaterialPreviewWindow::SetupCamera()
{
	// Unit sphere/plane at origin — camera 3 units away
	m_Camera.SetupCameraView(3.f, glm::vec3(0.f), 45.f, 35.f);
	m_Camera.SetupCameraProjection(2.f, 4.f, 1.f, 40.f);
	m_Camera.Update();
}

//=================================================================================
void C_MaterialPreviewWindow::UploadStorage()
{
	if (!m_Renderer)
		return;

	if (m_ImageLock.try_lock())
	{
		if (m_Renderer->NewResultAvailable())
		{
			Core::C_Application::Get().GetActiveRenderer().SetTextureData(m_GPUImageHandle, m_ImageStorage);
			m_Renderer->SetResultConsumed();
		}
		m_ImageLock.unlock();
	}
}

//=================================================================================
void C_MaterialPreviewWindow::Update()
{
	if (m_RebuildPending.load())
	{
		m_RebuildPending.store(false);
		RebuildAndRestart();
		return;
	}

	if (!IsVisible() && !m_Running.load())
	{
		m_WantToBeDestroyed = true;
		return;
	}
	UploadStorage();
}

//=================================================================================
void C_MaterialPreviewWindow::DrawComponents() const
{
	if (m_Material.IsReady() && m_Material.GetResource().IsModified())
	{
		ImGui::Text("Material modified");
	}
	std::ignore = m_GUIImage.Draw();

	auto* self = const_cast<C_MaterialPreviewWindow*>(this);

	// Shape selector
	ImGui::Text("Shape:");
	ImGui::SameLine();
	if (ImGui::RadioButton("Sphere", m_PreviewShape == E_PreviewShape::Sphere) && m_PreviewShape != E_PreviewShape::Sphere)
	{
		self->m_PreviewShape = E_PreviewShape::Sphere;
		self->m_RebuildPending.store(true);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Plane", m_PreviewShape == E_PreviewShape::Plane) && m_PreviewShape != E_PreviewShape::Plane)
	{
		self->m_PreviewShape = E_PreviewShape::Plane;
		self->m_RebuildPending.store(true);
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
	if (m_Material.IsReady())
		self->m_Material.GetResource().DrawGUI();

	ImGui::Separator();

	// Render progress
	const int samples = m_NumSamples.load();
	if (m_Running.load())
	{
		ImGui::ProgressBar(static_cast<float>(samples) / s_TargetSamples, ImVec2(-1.f, 0.f));
		ImGui::Text("Rendering... %d / %d samples", samples, s_TargetSamples);
	}
	else
	{
		ImGui::Text("Done — %d samples", samples);
		if (ImGui::Button("Re-render"))
			self->StartRender();
	}

	if (m_bWaitingForModal)
	{
		ImGui::OpenPopup("Confirm##Close");
		self->m_bWaitingForModal = false;
	}

	if (ImGui::BeginPopupModal("Confirm##Close", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Material was modified, are you sure you want to close?");
		ImGui::Separator();

		if (ImGui::Button("Yes", ImVec2(120, 0)))
		{
			// TODO should this be just on working copy and not on a live one?
			self->m_Material.GetResource().Reload();
			self->m_Material = {};
			self->SetVisible(false);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(120, 0)))
		{
			self->m_Material.GetResource().Save();
			self->m_Material = {};
			self->SetVisible(false);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

//=================================================================================
void C_MaterialPreviewWindow::RebuildAndRestart()
{
	m_StopRequested.store(true);
	while (m_Running.load())
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	m_StopRequested.store(false);

	constexpr glm::vec4 black{0.f, 0.f, 0.f, 1.f};
	Renderer::C_TextureView(&m_ImageStorage).ClearColor(black);
	Renderer::C_TextureView(&m_SamplesStorage).ClearColor(black);

	SetupScene();
	StartRender();
}

//=================================================================================
void C_MaterialPreviewWindow::NewMaterial()
{
}

//=================================================================================
void C_MaterialPreviewWindow::SaveMaterial()
{
	if (m_Material.IsReady())
		m_Material.GetResource().Save();
}

//=================================================================================
void C_MaterialPreviewWindow::SaveMaterialAs()
{
	if (m_Material.IsReady() == false)
	{
		return;
	}
	const auto dialogGUID = NextGUID();
	auto*	   dialog	  = new GUI::C_FileDialogWindow(
		 ".glmat", "Save material as...",
		 [this, dialogGUID](const std::filesystem::path& savePath, GUI::C_GUIManager& guiMgr) {
			 // auto& rm		   = Core::C_ResourceManager::Instance();
			 // auto  newResource = rm.CreateNewResource<Renderer::MaterialResource>(savePath);
			 // if (newResource.IsReady())
			 //{
			 //	 newResource.GetResource() = m_Material.GetResource();
			 //}
			 // m_Material.GetResource().Reload();
			 // m_Material = newResource;
			 // m_Material.GetResource().Save();

			 guiMgr.DestroyWindow(dialogGUID);
		 },
		 dialogGUID, Renderer::TextureResource::GetResourceDataPath());
	m_GUIManager.AddCustomWindow(dialog);
	dialog->SetVisible();
}

//=================================================================================
void C_MaterialPreviewWindow::StartRender()
{
	if (m_Running.load())
		return;

	m_NumSamples.store(0);
	m_StopRequested.store(false);
	m_Running.store(true);

	std::thread([this]() {
		while (!m_StopRequested.load())
		{
			const int samplesBefore = m_NumSamples.load();
			if (samplesBefore >= s_TargetSamples)
				break;

			m_Renderer->Render(m_Camera, m_ImageStorage, m_SamplesStorage, &m_ImageLock, samplesBefore,
							   Renderer::C_InterleavedLinesFactory{4});
			m_NumSamples.fetch_add(1);
		}
		m_Running.store(false);
	}).detach();
}

//=================================================================================
C_MaterialPreviewWindow::~C_MaterialPreviewWindow()
{
	m_StopRequested.store(true);
	while (m_Running.load())
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

	auto& rm = Core::C_Application::Get().GetActiveRenderer().GetRM();
	rm.destoryTexture(m_GPUImageHandle);
}

//=================================================================================
void C_MaterialPreviewWindow::RequestDestroy()
{
	m_StopRequested.store(true);
}

//=================================================================================
void C_MaterialPreviewWindow::OnHide()
{
	// we postpone hiding, if material was modified
	if (m_Material.IsReady() && m_Material.GetResource().IsModified())
	{
		m_bWaitingForModal = true;
		m_IsVisible		   = true;
	}
	else
	{
		C_Window::OnHide();
	}
}

//=================================================================================
bool C_MaterialPreviewWindow::CanDestroy() const
{
	return !m_Running.load();
}

} // namespace GLEngine::Editor
