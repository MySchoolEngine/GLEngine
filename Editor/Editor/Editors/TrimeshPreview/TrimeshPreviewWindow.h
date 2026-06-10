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
class C_TrimeshModel;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {

class EDITOR_API_EXPORT C_TrimeshPreviewWindow final : public GUI::C_Window {
public:
	C_TrimeshPreviewWindow(GUID guid, GUI::C_GUIManager& guiMGR);
	~C_TrimeshPreviewWindow() override;

	C_TrimeshPreviewWindow(const C_TrimeshPreviewWindow&)			= delete;
	C_TrimeshPreviewWindow(C_TrimeshPreviewWindow&&) noexcept		= delete;
	C_TrimeshPreviewWindow& operator=(const C_TrimeshPreviewWindow&) = delete;
	C_TrimeshPreviewWindow& operator=(C_TrimeshPreviewWindow&&) noexcept = delete;

	// Opens the model in a new tab. Switches focus if already open.
	void OpenModel(Core::ResourceHandle<Renderer::C_TrimeshModel> handle);

	void			   RequestDestroy() override;
	[[nodiscard]] bool CanDestroy() const override;
	void			   Update() override;

private:
	static constexpr int		s_TargetSamples = 128;
	static constexpr glm::uvec2 s_Resolution{512, 512};

	// Non-moveable per-tab render state, heap-allocated via unique_ptr in S_TrimeshTab.
	struct S_TrimeshTabData {
		Core::ResourceHandle<Renderer::C_TrimeshModel> m_Model;

		Renderer::C_RayTraceScene		   m_Scene;
		Renderer::Cameras::C_OrbitalCamera m_Camera; // PER-TAB — geometry-dependent
		S_RayPreviewState				   m_Render;  // GPU handle, storage, image viewer, render thread state
	};

	// Satisfies TabbedViewTab. Moveable because S_TrimeshTabData is behind a unique_ptr.
	struct S_TrimeshTab {
		std::string							  m_TabLabel;
		bool								  m_bModified = false; // always false — read-only
		std::unique_ptr<S_TrimeshTabData>	  m_Data;
	};

	void DrawComponents() const override;
	void DrawTabContent(const S_TrimeshTab& tab) const;

	void SetupScene(S_TrimeshTabData& data);
	void SetupCamera(S_TrimeshTabData& data);
	void StartRender(S_TrimeshTabData& data);
	void DestroyTabResources(S_TrimeshTab& tab);

	mutable GUI::C_TabbedView<S_TrimeshTab> m_TabbedView;
	GUI::C_GUIManager&						m_GUIManager;
};

} // namespace GLEngine::Editor
