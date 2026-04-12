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
class C_TrimeshModel;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {

class EDITOR_API_EXPORT C_TrimeshPreviewWindow final : public GUI::C_Window {
public:
	C_TrimeshPreviewWindow(GUID guid, GUI::C_GUIManager& guiMGR, Core::ResourceHandle<Renderer::C_TrimeshModel> model);
	~C_TrimeshPreviewWindow() override;

	C_TrimeshPreviewWindow(const C_TrimeshPreviewWindow& other)		= delete;
	C_TrimeshPreviewWindow(C_TrimeshPreviewWindow&& other) noexcept = delete;
	C_TrimeshPreviewWindow& operator=(const C_TrimeshPreviewWindow& other) = delete;
	C_TrimeshPreviewWindow& operator=(C_TrimeshPreviewWindow&& other) noexcept = delete;

	void			   RequestDestroy() override;
	[[nodiscard]] bool CanDestroy() const override;
	void			   Update() override;

private:
	void DrawComponents() const override;
	void SetupScene();
	void SetupCamera(const Physics::Primitives::S_AABB& combinedAABB);
	void StartRender();
	void UploadStorage();

	Core::ResourceHandle<Renderer::C_TrimeshModel> m_Model;
	Renderer::C_RayTraceScene					   m_Scene;
	Renderer::Cameras::C_OrbitalCamera			   m_Camera;
	std::unique_ptr<Renderer::C_RayRenderer>	   m_Renderer;

	Renderer::Handle<Renderer::Texture>		 m_GPUImageHandle;
	Renderer::C_TextureViewStorageCPU<float> m_ImageStorage;
	Renderer::C_TextureViewStorageCPU<float> m_SamplesStorage;
	GUI::C_ImageViewer						 m_GUIImage;

	std::mutex		  m_ImageLock;
	std::atomic<int>  m_NumSamples{0};
	std::atomic<bool> m_Running{false};
	std::atomic<bool> m_StopRequested{false};

	static constexpr int		s_TargetSamples = 128;
	static constexpr glm::uvec2 s_Resolution{512, 512};
};

} // namespace GLEngine::Editor
