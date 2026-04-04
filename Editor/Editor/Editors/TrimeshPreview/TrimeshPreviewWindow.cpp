#include <EditorStdafx.h>

#include <Editor/Editors/TrimeshPreview/TrimeshPreviewWindow.h>

#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/RayCasting/Geometry/TrimeshModel.h>
#include <Renderer/RayCasting/Geometry/PrimitiveObject.h>
#include <Renderer/RayCasting/Light/RayAreaLight.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Colours.h>
#include <Renderer/Textures/TextureView.h>
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
C_TrimeshPreviewWindow::C_TrimeshPreviewWindow(GUID guid,
											   GUI::C_GUIManager&                              guiMGR,
											   Core::ResourceHandle<Renderer::C_TrimeshModel> model)
	: GUI::C_Window(guid, "Trimesh Preview")
	, m_Model(std::move(model))
	, m_ImageStorage(s_Resolution.x, s_Resolution.y, 3)
	, m_SamplesStorage(s_Resolution.x, s_Resolution.y, 3)
	, m_GUIImage({})
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();

	m_GPUImageHandle = renderer.GetRM().createTexture(Renderer::TextureDescriptor{
		.name		  = "trimeshPreview",
		.width		  = s_Resolution.x,
		.height		  = s_Resolution.y,
		.type		  = Renderer::E_TextureType::TEXTURE_2D,
		.format		  = Renderer::E_TextureFormat::RGB32f,
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
void C_TrimeshPreviewWindow::SetupScene()
{
	m_Scene.ClearScene();

	Physics::Primitives::S_AABB combinedAABB;
	for (const auto& trimesh : m_Model.GetResource().GetTrimeshes())
		combinedAABB.Add(trimesh.GetAABB());

	static const Renderer::MeshData::Material s_White{
		glm::vec4{}, glm::vec4{Colours::white, 0.f}, glm::vec4{}, 0.f, -1, -1, "white"};
	m_Scene.AddMesh(m_Model, s_White); // identity transform (default from Task 1)

	const auto    sphere = combinedAABB.GetSphere();
	const float   r      = sphere.m_radius;
	const glm::vec3 c    = sphere.m_position;

	// One overhead disc area light centred above the mesh
	static const Renderer::MeshData::Material s_Black{
		glm::vec4{}, glm::vec4{Colours::black, 0.f}, glm::vec4{}, 0.f, -1, -1, "black"};

	const glm::vec3 lightNormal = glm::normalize(glm::vec3(0.f, -1.f, 0.f));
	auto disc = Physics::Primitives::S_Disc(lightNormal, c + glm::vec3(0.f, r * 3.f, 0.f), r * 1.5f);
	disc.plane.twoSided = false;
	auto discPrimitive  = std::make_shared<Renderer::C_Primitive<Physics::Primitives::S_Disc>>(disc);
	discPrimitive->SetMaterial(m_Scene.AddMaterial(s_Black).get());
	m_Scene.AddLight(std::make_shared<Renderer::RayTracing::C_AreaLight>(
		glm::vec3(1.f, 1.f, 1.f) * 5.f, discPrimitive));

	SetupCamera(combinedAABB);

	m_Renderer = std::make_unique<Renderer::C_RayRenderer>(m_Scene);
}

//=================================================================================
void C_TrimeshPreviewWindow::SetupCamera(const Physics::Primitives::S_AABB& combinedAABB)
{
	const auto    sphere   = combinedAABB.GetSphere();
	const float   r        = sphere.m_radius > 0.f ? sphere.m_radius : 1.f;
	const glm::vec3 center = sphere.m_position;

	const float distance = r * 3.f;

	const float nearZ = std::max(0.01f, distance - r);
	const float farZ  = distance + r;
	const float fovY  = glm::degrees(2.f * std::atan((r * 1.1f) / distance));

	// Top-right view: 45° azimuth, 35° elevation
	m_Camera.SetupCameraView(distance, center, 45.f, 35.f);
	m_Camera.SetupCameraProjection(nearZ, farZ, 1.f /*square aspect*/, fovY);
	m_Camera.Update();
}

//=================================================================================
void C_TrimeshPreviewWindow::UploadStorage()
{
	if (!m_Renderer)
		return;

	if (m_ImageLock.try_lock())
	{
		if (m_Renderer->NewResultAvailable())
		{
			Core::C_Application::Get().GetActiveRenderer()
				.SetTextureData(m_GPUImageHandle, m_ImageStorage);
			m_Renderer->SetResultConsumed();
		}
		m_ImageLock.unlock();
	}
}

//=================================================================================
void C_TrimeshPreviewWindow::Update()
{
	if (!IsVisible() && !m_Running.load())
	{
		m_WantToBeDestroyed = true;
		return;
	}
	UploadStorage();
}

//=================================================================================
void C_TrimeshPreviewWindow::DrawComponents() const
{
	m_GUIImage.Draw();

	const int samples = m_NumSamples.load();
	if (m_Running.load())
	{
		ImGui::ProgressBar(static_cast<float>(samples) / s_TargetSamples,
						   ImVec2(-1.f, 0.f));
		ImGui::Text("Rendering... %d / %d samples", samples, s_TargetSamples);
	}
	else
	{
		ImGui::Text("Done — %d samples", samples);
		if (ImGui::Button("Re-render"))
			const_cast<C_TrimeshPreviewWindow*>(this)->StartRender();
	}
}

//=================================================================================
void C_TrimeshPreviewWindow::StartRender()
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

			m_Renderer->Render(m_Camera,
							   m_ImageStorage,
							   m_SamplesStorage,
							   &m_ImageLock,
							   samplesBefore);
			m_NumSamples.fetch_add(1);
		}
		m_Running.store(false);
	}).detach();
}

//=================================================================================
C_TrimeshPreviewWindow::~C_TrimeshPreviewWindow()
{
	m_StopRequested.store(true);
	while (m_Running.load())
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

	auto& rm = Core::C_Application::Get().GetActiveRenderer().GetRM();
	rm.destoryTexture(m_GPUImageHandle);
}

//=================================================================================
void C_TrimeshPreviewWindow::RequestDestroy()
{
	GUI::C_Window::RequestDestroy();
	m_StopRequested.store(true);
}

//=================================================================================
bool C_TrimeshPreviewWindow::CanDestroy() const
{
	return !m_Running.load();
}

} // namespace GLEngine::Editor
