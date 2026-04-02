#include <EditorStdafx.h>

#include <Editor/Editors/TrimeshPreview/TrimeshPreviewWindow.h>

#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/RayCasting/Geometry/TrimeshModel.h>
#include <Renderer/RayCasting/Light/RayPointLight.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Colours.h>
#include <Renderer/Textures/TextureView.h>

#include <Physics/Primitives/AABB.h>

#include <Core/Application.h>

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

	static const MeshData::Material s_White{
		glm::vec4{}, glm::vec4{Renderer::Colours::white, 0.f}, glm::vec4{}, 0.f, -1, -1, "white"};
	m_Scene.AddMesh(m_Model, s_White); // identity transform (default from Task 1)

	const auto    sphere = combinedAABB.GetSphere();
	const float   r      = sphere.radius;
	const glm::vec3 c    = sphere.position;

	// Key light: warm, top-right-front
	m_Scene.AddLight(std::make_shared<Renderer::RayTracing::C_PointLight>(
		c + glm::vec3(r * 2.f, r * 2.f, r * 2.f),
		glm::vec3(1.f, 0.95f, 0.9f) * 2.f));

	// Fill light: cool, top-left-front
	m_Scene.AddLight(std::make_shared<Renderer::RayTracing::C_PointLight>(
		c + glm::vec3(-r * 2.f, r * 1.f, r * 2.f),
		glm::vec3(0.8f, 0.85f, 1.f)));

	// Rim/back light: neutral, high behind
	m_Scene.AddLight(std::make_shared<Renderer::RayTracing::C_PointLight>(
		c + glm::vec3(0.f, r * 1.5f, -r * 3.f),
		glm::vec3(1.f, 1.f, 1.f) * 1.5f));

	SetupCamera(combinedAABB);

	m_Renderer = std::make_unique<Renderer::C_RayRenderer>(m_Scene);
}

//=================================================================================
void C_TrimeshPreviewWindow::SetupCamera(const Physics::Primitives::S_AABB& combinedAABB)
{
	const auto    sphere   = combinedAABB.GetSphere();
	const float   r        = sphere.radius > 0.f ? sphere.radius : 1.f;
	const glm::vec3 center = sphere.position;

	const float distance = r * 3.f;

	const float nearZ = std::max(0.01f, distance - r);
	const float farZ  = distance + r;
	const float fovY  = glm::degrees(2.f * std::atan((r * 1.1f) / distance));

	// Top-right view: 45° azimuth, 35° elevation
	m_Camera.SetupCameraView(distance, center, 45.f, 35.f);
	m_Camera.SetupCameraProjection(nearZ, farZ, 1.f /*square aspect*/, fovY);
	m_Camera.Update();
}

} // namespace GLEngine::Editor
