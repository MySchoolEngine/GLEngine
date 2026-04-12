#pragma once

#include <Editor/EditorApi.h>

#include <GUI/GUIWindow.h>

#include <Core/EventSystem/Event.h>

#include <atomic>
#include <filesystem>
#include <functional>
#include <thread>
#include <vector>

namespace GLEngine::GUI {
class C_GUIManager;
} // namespace GLEngine::GUI

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

	void OnFolderSelected(const std::filesystem::path& path) const;
	void OnResourceDoubleClicked(const std::filesystem::path& path) const;
	void HandleResourceDragDrop(const std::filesystem::path& path, float iconSize) const;
	void HandleContextMenu(const std::filesystem::path& path) const;
	void ExportTrimesh(const std::filesystem::path& path) const;

	void StartWatcher(const std::filesystem::path& path) const;
	void StopWatcher() const;

	std::filesystem::path					   m_RootPath;
	mutable std::filesystem::path			   m_SelectedFolder;
	mutable std::vector<std::filesystem::path> m_FolderContents;
	mutable bool							   m_ContentDirty = true;

	mutable std::thread		  m_WatcherThread;
	mutable std::atomic<bool> m_WatcherRunning{false};
	mutable std::atomic<bool> m_ChangePending{false};

	GUI::C_GUIManager& m_GUIManager;
	T_EventCallback	   m_EventCallback;
	mutable GUID	   m_ImageEditorGUID;
	mutable GUID	   m_TrimeshPreviewGUID;
};

} // namespace GLEngine::Editor
