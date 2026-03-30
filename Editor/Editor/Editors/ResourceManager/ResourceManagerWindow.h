#pragma once

#include <Editor/EditorApi.h>

#include <GUI/GUIWindow.h>

#include <atomic>
#include <filesystem>
#include <thread>
#include <vector>

namespace GLEngine::GUI {
class C_GUIManager;
} // namespace GLEngine::GUI

namespace GLEngine::Editor {

class EDITOR_API_EXPORT C_ResourceManagerWindow final : public GUI::C_Window {
public:
	C_ResourceManagerWindow(GUID guid, GUI::C_GUIManager& guiMgr, std::filesystem::path rootPath);
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

	void StartWatcher(const std::filesystem::path& path) const;
	void StopWatcher() const;

	std::filesystem::path						  m_RootPath;
	mutable std::filesystem::path				  m_SelectedFolder;
	mutable std::vector<std::filesystem::path>	  m_FolderContents;
	mutable bool								  m_ContentDirty = true;

	mutable std::thread		  m_WatcherThread;
	mutable std::atomic<bool> m_WatcherRunning{false};
	mutable std::atomic<bool> m_ChangePending{false};

	GUI::C_GUIManager& m_GUIManager;
};

} // namespace GLEngine::Editor
