#pragma once

#include <Editor/EditorApi.h>
#include <Editor/Editors/ImageEditorTool.h>

#include <Renderer/Textures/Storage/TextureLinearStorage.h>
#include <Renderer/Textures/TextureResource.h>

#include <GUI/GUIWindow.h>
#include <GUI/ImageViewer.h>

#include <Core/EventSystem/Event.h>
#include <Core/Resources/ResourceHandle.h>

#include <functional>
#include <memory>
#include <optional>
#include <vector>

namespace GLEngine::Renderer {
class I_DeviceTexture;
class I_Renderer;
class TextureResource;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {

class EDITOR_API_EXPORT C_ImageEditor : public GUI::C_Window {
public:
	using T_EventCallback = std::function<void(Core::I_Event&)>;

	C_ImageEditor(GUID guid, GUI::C_GUIManager& guiMGR, T_EventCallback eventCallback = {}, Core::ResourceHandle<Renderer::TextureResource> initialResource = {});
	~C_ImageEditor() override;

	C_ImageEditor(const C_ImageEditor&)			   = delete;
	C_ImageEditor& operator=(const C_ImageEditor&) = delete;

	// Opens a resource as a new tab. If already open, switches focus.
	void OpenImage(Core::ResourceHandle<Renderer::TextureResource> handle);
	// Creates a blank canvas as a new tab (no backing resource until Save As).
	void NewImage(std::uint32_t width = 1024, std::uint32_t height = 1024);

	void Update() override;
	bool CanDestroy() const override;

protected:
	void DrawComponents() const override;

private:
	struct S_ImageTab {
		Core::ResourceHandle<Renderer::TextureResource>			m_ResourceHandle;
		std::string												m_TabLabel;
		bool													m_bModified = false;
		std::optional<Renderer::C_TextureViewStorageCPU<float>> m_Storage;
		Renderer::Handle<Renderer::Texture>						m_DeviceImage;
		Renderer::C_TextureViewStorageCPU<float>				m_Histogram{255, 1, 1};
		std::optional<GUI::C_ImageViewer>						m_GUIImage;
		std::unique_ptr<C_ImageEditorTool>						m_ActiveTool;
		bool													m_bNeedsSync = false;
	};

	S_ImageTab&		  ActiveTab();
	const S_ImageTab& ActiveTab() const;

	// Returns reference to the newly created tab. Valid until next CreateTab call.
	S_ImageTab& CreateTab(Core::ResourceHandle<Renderer::TextureResource> handle, std::uint32_t width = 1024, std::uint32_t height = 1024);
	void		DestroyTabResources(S_ImageTab& tab) const;
	void		SyncTabToGPU(const S_ImageTab& tab) const;
	void		SaveTab(S_ImageTab& tab);
	void		SaveTabAs(S_ImageTab& tab);

	void DrawTabContent(const S_ImageTab& tab) const;

	// Shared checkerboard background across all tabs
	Renderer::Handle<Renderer::Texture> m_Background;
	GUI::Menu::C_Menu					m_FileMenu;
	GUI::Menu::C_Menu					m_ToolsdMenu;
	GUI::C_GUIManager&					m_GUIManager;
	T_EventCallback						m_EventCallback;

	// unique_ptr per tab so the vector is moveable regardless of S_ImageTab's move semantics
	mutable std::vector<S_ImageTab> m_Tabs;
	mutable int										 m_ActiveTabIndex = 0;
};
} // namespace GLEngine::Editor
