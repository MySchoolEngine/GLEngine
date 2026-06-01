#pragma once

#include <Editor/EditorApi.h>

#include <Renderer/Cameras/OrbitalCamera.h>
#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>

#include <GUI/GUIWindow.h>
#include <GUI/ImageViewer.h>

#include <Core/Resources/ResourceHandle.h>

#include <atomic>
#include <mutex>

namespace GLEngine::Renderer {
class MaterialResource;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {

enum class E_PreviewShape
{
	Sphere,
	Plane,
};

class EDITOR_API_EXPORT C_MaterialPreviewWindow final : public GUI::C_Window {
public:
	C_MaterialPreviewWindow(GUID guid, GUI::C_GUIManager& guiMGR, Core::ResourceHandle<Renderer::MaterialResource> material);
	~C_MaterialPreviewWindow() override;

	C_MaterialPreviewWindow(const C_MaterialPreviewWindow& other)		= delete;
	C_MaterialPreviewWindow(C_MaterialPreviewWindow&& other) noexcept	= delete;
	C_MaterialPreviewWindow& operator=(const C_MaterialPreviewWindow& other) = delete;
	C_MaterialPreviewWindow& operator=(C_MaterialPreviewWindow&& other) noexcept = delete;

	void			   RequestDestroy() override;
	[[nodiscard]] bool CanDestroy() const override;
	void			   Update() override;

private:
	void DrawComponents() const override;
	void SetupScene();
	void SetupCamera();
	void StartRender();
	void UploadStorage();
	void RebuildAndRestart();

	void NewMaterial();
	void SaveMaterial();
	void SaveMaterialAs();

	Core::ResourceHandle<Renderer::MaterialResource> m_Material;
	E_PreviewShape									 m_PreviewShape{E_PreviewShape::Sphere};
	std::atomic<bool>								 m_RebuildPending{false};

	Renderer::C_RayTraceScene				   m_Scene;
	Renderer::Cameras::C_OrbitalCamera		   m_Camera;
	std::unique_ptr<Renderer::C_RayRenderer>   m_Renderer;

	Renderer::Handle<Renderer::Texture>		 m_GPUImageHandle;
	Renderer::C_TextureViewStorageCPU<float> m_ImageStorage;
	Renderer::C_TextureViewStorageCPU<float> m_SamplesStorage;
	GUI::C_ImageViewer						 m_GUIImage;
	GUI::Menu::C_Menu						 m_FileMenu;
	GUI::C_GUIManager&						 m_GUIManager;

	std::mutex		  m_ImageLock;
	std::atomic<int>  m_NumSamples{0};
	std::atomic<bool> m_Running{false};
	std::atomic<bool> m_StopRequested{false};

	// GPU rasterizer placeholder — extend this enum when GPU path is implemented
	enum class E_RenderMode
	{
		CPU,
		// GPU,
	};
	E_RenderMode m_RenderMode{E_RenderMode::CPU};

	static constexpr int		s_TargetSamples = 128;
	static constexpr glm::uvec2 s_Resolution{512, 512};
};

} // namespace GLEngine::Editor
