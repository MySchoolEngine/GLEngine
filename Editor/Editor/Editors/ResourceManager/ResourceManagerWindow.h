#pragma once

#include <Editor/EditorApi.h>

#include <GUI/GUIWindow.h>

#include <filesystem>
#include <vector>

namespace GLEngine::GUI {
class C_GUIManager;
} // namespace GLEngine::GUI

namespace GLEngine::Editor {

class EDITOR_API_EXPORT C_ResourceManagerWindow final : public GUI::C_Window {
public:
	C_ResourceManagerWindow(GUID guid, GUI::C_GUIManager& guiMgr, std::filesystem::path rootPath);

	void Update() override;

protected:
	void DrawComponents() const override;

private:
	void DrawFolderTree(const std::filesystem::path& dir) const;
	void DrawContentPanel() const;

	void OnFolderSelected(const std::filesystem::path& path) const;
	void OnResourceDoubleClicked(const std::filesystem::path& path) const;
	void HandleResourceDragDrop(const std::filesystem::path& path) const;

	std::filesystem::path						  m_RootPath;
	mutable std::filesystem::path				  m_SelectedFolder;
	mutable std::vector<std::filesystem::path>	  m_FolderContents;
	mutable bool								  m_ContentDirty = true;

	GUI::C_GUIManager& m_GUIManager;
};

} // namespace GLEngine::Editor
