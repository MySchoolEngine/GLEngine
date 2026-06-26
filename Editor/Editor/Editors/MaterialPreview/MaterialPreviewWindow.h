#pragma once

#include <Editor/EditorApi.h>
#include <Editor/Editors/RayPreviewState.h>

#include <Renderer/Cameras/OrbitalCamera.h>
#include <Renderer/RayCasting/Geometry/RayTraceScene.h>

#include <GUI/GUIWindow.h>
#include <GUI/TabbedView.h>

#include <Core/Resources/ResourceHandle.h>

#include <atomic>
#include <memory>

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
	C_MaterialPreviewWindow(GUID guid, GUI::C_GUIManager& guiMGR);
	~C_MaterialPreviewWindow() override;

	C_MaterialPreviewWindow(const C_MaterialPreviewWindow&)		= delete;
	C_MaterialPreviewWindow(C_MaterialPreviewWindow&&) noexcept = delete;
	C_MaterialPreviewWindow& operator=(const C_MaterialPreviewWindow&) = delete;
	C_MaterialPreviewWindow& operator=(C_MaterialPreviewWindow&&) noexcept = delete;

	// Opens the material in a new tab. Switches focus if already open.
	void OpenMaterial(Core::ResourceHandle<Renderer::MaterialResource> handle);

	void			   RequestDestroy() override;
	[[nodiscard]] bool CanDestroy() const override;
	void			   Update() override;
	void			   OnHide() override;

private:
	static constexpr int		s_TargetSamples = 128;
	static constexpr glm::uvec2 s_Resolution{512, 512};

	// GPU rasterizer placeholder — extend when GPU path is implemented
	enum class E_RenderMode
	{
		CPU,
		// GPU,
	};

	// Non-moveable per-tab render state, heap-allocated via unique_ptr in S_MaterialTab.
	struct S_MaterialTabData {
		Core::ResourceHandle<Renderer::MaterialResource> m_Material;
		E_PreviewShape									 m_PreviewShape{E_PreviewShape::Sphere};
		E_RenderMode									 m_RenderMode{E_RenderMode::CPU};
		std::atomic<bool>								 m_RebuildPending{false};

		Renderer::C_RayTraceScene m_Scene;
		S_RayPreviewState		  m_Render; // GPU handle, storage, image viewer, render thread state
	};

	// Satisfies TabbedViewTab. Moveable because S_MaterialTabData is behind a unique_ptr.
	struct S_MaterialTab {
		std::string						   m_TabLabel;
		bool							   m_bModified = false;
		std::unique_ptr<S_MaterialTabData> m_Data;
	};

	void DrawComponents() const override;
	void DrawTabContent(const S_MaterialTab& tab) const;

	void SetupScene(S_MaterialTabData& data);
	void SetupCamera();
	void StartRender(S_MaterialTabData& data);
	void RebuildAndRestart(S_MaterialTabData& data);
	void DestroyTabResources(S_MaterialTab& tab);

	void NewMaterial();
	void SaveMaterial(S_MaterialTabData& data);
	void SaveMaterialAs(S_MaterialTabData& data);

	mutable GUI::C_TabbedView<S_MaterialTab> m_TabbedView;

	Renderer::Cameras::C_OrbitalCamera m_Camera; // shared — fixed viewpoint, same for all tabs
	GUI::Menu::C_Menu				   m_FileMenu;
	GUI::C_GUIManager&				   m_GUIManager;
};

} // namespace GLEngine::Editor
