#include <EditorStdafx.h>

#include <Editor/Editors/ResourceManager/ResourceManagerWindow.h>

#include <imgui.h>

// TODO First of all, for the drag and drop, I will need to define the IDs for draggable things inside of the editor.
// It will need to contain the information about the drug thing being resourced and its type. So it will be tied to the resource type.
// Secondly, I need to update the UI that is showing the resource inside of, for example, the component view or something like that.
// Next, I will need to tie the editor to the resource type. This will be done in editor Module anyway. It will tie the C++ type to the editor class.

namespace GLEngine::Editor {

//=================================================================================
C_ResourceManagerWindow::C_ResourceManagerWindow(GUID guid, GUI::C_GUIManager& guiMgr, std::filesystem::path rootPath)
	: GUI::C_Window(guid, "Resource Manager")
	, m_RootPath(std::move(rootPath))
	, m_SelectedFolder(m_RootPath)
	, m_GUIManager(guiMgr)
{
}

//=================================================================================
void C_ResourceManagerWindow::Update()
{
	// placeholder for future filesystem watcher
}

//=================================================================================
void C_ResourceManagerWindow::DrawComponents() const
{
	const float totalWidth	  = ImGui::GetContentRegionAvail().x;
	const float folderPanelW  = totalWidth * 0.3f;
	const float contentPanelW = totalWidth - folderPanelW - ImGui::GetStyle().ItemSpacing.x;

	ImGui::BeginChild("##FolderTree", ImVec2(folderPanelW, 0)/*, ImGuiChildFlags_Border*/);
	DrawFolderTree(m_RootPath);
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("##ContentPanel", ImVec2(contentPanelW, 0)/*, ImGuiChildFlags_Border*/);
	DrawContentPanel();
	ImGui::EndChild();
}

//=================================================================================
void C_ResourceManagerWindow::DrawFolderTree(const std::filesystem::path& dir) const
{
	std::error_code ec;
	for (const auto& entry : std::filesystem::directory_iterator(dir, ec))
	{
		if (!entry.is_directory())
			continue;

		const bool isSelected  = (entry.path() == m_SelectedFolder);
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (isSelected)
			flags |= ImGuiTreeNodeFlags_Selected;

		// Check if this directory has subdirectories
		bool hasSubDirs = false;
		for (const auto& sub : std::filesystem::directory_iterator(entry.path(), ec))
		{
			if (sub.is_directory())
			{
				hasSubDirs = true;
				break;
			}
		}
		if (!hasSubDirs)
			flags |= ImGuiTreeNodeFlags_Leaf;

		const bool nodeOpen = ImGui::TreeNodeEx(entry.path().filename().string().c_str(), flags);

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			OnFolderSelected(entry.path());

		if (nodeOpen)
		{
			DrawFolderTree(entry.path());
			ImGui::TreePop();
		}
	}
}

//=================================================================================
void C_ResourceManagerWindow::DrawContentPanel() const
{
	if (m_ContentDirty)
	{
		m_FolderContents.clear();
		std::error_code ec;
		for (const auto& entry : std::filesystem::directory_iterator(m_SelectedFolder, ec))
			m_FolderContents.push_back(entry.path());
		m_ContentDirty = false;
	}

	for (const auto& path : m_FolderContents)
	{
		const std::string name = path.filename().string();
		// TODO find here the resource type and add its name
		// TODO for folders write it there
		ImGui::Selectable(name.c_str(), false);

		HandleResourceDragDrop(path);

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (std::filesystem::is_directory(path))
				OnFolderSelected(path);
			else
				OnResourceDoubleClicked(path);
		}
	}
}

//=================================================================================
void C_ResourceManagerWindow::OnFolderSelected(const std::filesystem::path& path) const
{
	m_SelectedFolder = path;
	m_ContentDirty	 = true;
}

//=================================================================================
void C_ResourceManagerWindow::OnResourceDoubleClicked(const std::filesystem::path& path) const
{
	// TODO: dispatch to appropriate editor based on file extension
	// e.g. .png/.jpg/.hdr -> C_ImageEditor
	// Use m_GUIManager to create and show the editor window
	(void)path;
}

//=================================================================================
void C_ResourceManagerWindow::HandleResourceDragDrop(const std::filesystem::path& path) const
{
	if (ImGui::BeginDragDropSource())
	{
		const std::string pathStr = path.string();
		ImGui::SetDragDropPayload("RESOURCE_PATH", pathStr.c_str(), pathStr.size() + 1);
		ImGui::Text("%s", path.filename().string().c_str());
		ImGui::EndDragDropSource();
	}
}

} // namespace GLEngine::Editor
