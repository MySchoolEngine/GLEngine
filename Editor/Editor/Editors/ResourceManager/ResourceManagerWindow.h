#pragma once

#include <Editor/EditorApi.h>

#include <GUI/GUIWindow.h>

#include <Core/EventSystem/Event.h>

#include <atomic>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace GLEngine::GUI {
class C_GUIManager;
} // namespace GLEngine::GUI

namespace GLEngine::Core {
class I_ResourceLoader;
} // namespace GLEngine::Core

namespace GLEngine::Editor {

class EDITOR_API_EXPORT C_ResourceManagerWindow final : public GUI::C_Window {
public:
	using T_EventCallback = std::function<void(Core::I_Event&)>;

	C_ResourceManagerWindow(GUID guid, GUI::C_GUIManager& guiMgr, std::filesystem::path rootPath, T_EventCallback eventCallback = {});
	~C_ResourceManagerWindow() override;

	void Update() override;

protected:
	void DrawComponents() const override;

private:
	void DrawFolderTree(const std::filesystem::path& dir) const;
	void DrawContentPanel() const;

	void DrawGridItem(const std::filesystem::path& path, float iconSize) const;
	void DrawListItem(const std::filesystem::path& path) const;
	void DrawPendingCreateRow() const;

	void OnFolderSelected(const std::filesystem::path& path) const;
	void OnResourceDoubleClicked(const std::filesystem::path& path) const;
	void HandleResourceDragDrop(const std::filesystem::path& path, float iconSize) const;
	void HandleContextMenu(const std::filesystem::path& path) const;
	void ExportTrimesh(const std::filesystem::path& path) const;
	void ExportMaterials(const std::filesystem::path& path, bool reexport) const;

	void DrawNewResourceMenuContents() const;
	void BeginCreate(const Core::I_ResourceLoader& loader) const;
	void BeginRename(const std::filesystem::path& path) const;
	bool IsEditingPath(const std::filesystem::path& path) const;
	void DrawInlineNameEditor(float width) const;
	void CommitPendingEdit() const;

	void StartWatcher(const std::filesystem::path& path) const;
	void StopWatcher() const;

	struct S_PendingEdit {
		bool						  m_IsCreate = false;		   // true: synthetic placeholder, not yet created. false: renaming an existing item at m_TargetPath.
		std::filesystem::path		  m_TargetPath;				   // create: folder the new resource is created in. rename: path of the existing item.
		const Core::I_ResourceLoader* m_CreateLoader	= nullptr; // only set when m_IsCreate
		char						  m_NameBuffer[128] = {};
		bool						  m_FocusPending	= true;
		std::string					  m_ErrorMessage;
	};
	mutable std::optional<S_PendingEdit> m_PendingEdit;

	std::filesystem::path					   m_RootPath;
	mutable std::filesystem::path			   m_SelectedFolder;
	mutable std::vector<std::filesystem::path> m_FolderContents;
	mutable bool							   m_ContentDirty = true;

	mutable char m_FilterName[128] = {};
	mutable int	 m_FilterTypeIndex = 0; // 0 = All
	mutable bool m_ListView		   = false;

	mutable std::thread		  m_WatcherThread;
	mutable std::atomic<bool> m_WatcherRunning{false};
	mutable std::atomic<bool> m_ChangePending{false};

	GUI::C_GUIManager& m_GUIManager;
	T_EventCallback	   m_EventCallback;
	mutable GUID	   m_ImageEditorGUID;
	mutable GUID	   m_TrimeshPreviewGUID;
	mutable GUID	   m_MaterialPreviewGUID;
};

} // namespace GLEngine::Editor
