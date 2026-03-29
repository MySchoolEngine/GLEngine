#include <EditorStdafx.h>

#include <Editor/Editors/ResourceManager/ResourceManagerWindow.h>
#include <Core/Resources/ResourceManager.h>

#include <imgui.h>
#include <IconsFontAwesome6.h>
#include <GUI/ImGuiLayer.h>

namespace {
const char* GetIconForPath(const std::filesystem::path& path)
{
	if (std::filesystem::is_directory(path))
		return ICON_FA_FOLDER;
	const auto ext = path.extension().string();
	if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".hdr" || ext == ".tga")
		return ICON_FA_FILE_IMAGE;
	if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb")
		return ICON_FA_CUBE;
	return ICON_FA_FILE;
}

void DrawIconCentered(ImDrawList* drawList, ImVec2 rectMin, float rectSize, const char* iconStr)
{
	ImFont*		 font	   = GLEngine::GUI::C_ImGuiLayer::GetLargeIconFont();
	const float	 fontSize  = font ? 32.0f : 16.0f;
	ImFont*		 renderFont = font ? font : ImGui::GetFont();
	const ImVec2 textSize  = renderFont->CalcTextSizeA(fontSize, FLT_MAX, 0.f, iconStr);
	const ImVec2 textPos(rectMin.x + (rectSize - textSize.x) * 0.5f, rectMin.y + (rectSize - textSize.y) * 0.5f);
	drawList->AddText(renderFont, fontSize, textPos, IM_COL32(200, 200, 200, 210), iconStr);
}
} // namespace

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

		const std::string treeLabel = std::string(ICON_FA_FOLDER) + "  " + entry.path().filename().string();
		const bool		  nodeOpen	= ImGui::TreeNodeEx(treeLabel.c_str(), flags);

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

	constexpr float iconSize = 64.0f;
	constexpr float padding	 = 12.0f;
	constexpr float cellW	 = iconSize + padding;
	const float		cellH	 = iconSize + ImGui::GetTextLineHeightWithSpacing() + 4.0f;
	const int		numCols	 = std::max(1, static_cast<int>(ImGui::GetContentRegionAvail().x / cellW));

	for (int i = 0; i < static_cast<int>(m_FolderContents.size()); i++)
	{
		const int col = i % numCols;
		const int row = i / numCols;
		ImGui::SetCursorPos(ImVec2(col * cellW, row * cellH));
		DrawGridItem(m_FolderContents[i], iconSize);
	}

	// Advance cursor past all rows so the child window scrolls correctly
	const int numRows = (static_cast<int>(m_FolderContents.size()) + numCols - 1) / numCols;
	ImGui::SetCursorPos(ImVec2(0.f, numRows * cellH));
	ImGui::Dummy(ImVec2(0.f, 0.f));
}

//=================================================================================
void C_ResourceManagerWindow::DrawGridItem(const std::filesystem::path& path, float iconSize) const
{
	const std::string name	  = path.filename().string();
	const bool		  isDir	  = std::filesystem::is_directory(path);
	const ImVec2	  iconPos = ImGui::GetCursorPos(); // capture before button for text centering

	ImGui::InvisibleButton(("##" + name).c_str(), ImVec2(iconSize, iconSize));

	const ImVec2 rMin		 = ImGui::GetItemRectMin();
	const ImVec2 rMax		 = ImGui::GetItemRectMax();
	const ImU32	 borderColor = ImGui::IsItemHovered() ? IM_COL32(200, 200, 200, 255) : IM_COL32(110, 110, 110, 255);
	ImGui::GetWindowDrawList()->AddRect(rMin, rMax, borderColor, 4.0f, 0, 1.5f);

	// Draw FA icon centered in the cell via draw list - no new ImGui item, InvisibleButton stays as drag source
	DrawIconCentered(ImGui::GetWindowDrawList(), rMin, iconSize, GetIconForPath(path));

	HandleResourceDragDrop(path, iconSize);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		if (isDir)
			OnFolderSelected(path);
		else
			OnResourceDoubleClicked(path);
	}

	constexpr int maxChars = 10;
	std::string	  label	   = name.size() > maxChars ? name.substr(0, maxChars - 2) + ".." : name;
	const float	  textW	   = ImGui::CalcTextSize(label.c_str()).x;
	ImGui::SetCursorPos(ImVec2(iconPos.x + std::max(0.f, (iconSize - textW) * 0.5f), ImGui::GetCursorPosY()));
	ImGui::TextUnformatted(label.c_str());
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
void C_ResourceManagerWindow::HandleResourceDragDrop(const std::filesystem::path& path, float iconSize) const
{
	const auto& resMgr = Core::C_ResourceManager::Instance();
	auto		loader = resMgr.GetLoaderForExt(path.extension().generic_string());
	if (loader.has_value() == false)
		return;

	if (ImGui::BeginDragDropSource())
	{
		const std::string pathStr = path.string();
		ImGui::SetDragDropPayload(loader->get().DragAndDropLabel().c_str(), pathStr.c_str(), pathStr.size() + 1);

		// Preview matches grid visuals: placeholder rect + truncated name, both centred
		constexpr int	  maxChars = 10;
		const std::string name	   = path.filename().string();
		const std::string label	   = name.size() > maxChars ? name.substr(0, maxChars - 2) + ".." : name;
		const float		  textW	   = ImGui::CalcTextSize(label.c_str()).x;
		const float		  cellW	   = std::max(iconSize, textW);

		const ImVec2 previewStart = ImGui::GetCursorPos();
		ImGui::SetCursorPosX(previewStart.x + (cellW - iconSize) * 0.5f);
		const ImVec2 iconScreenPos = ImGui::GetCursorScreenPos();
		ImGui::Dummy(ImVec2(iconSize, iconSize));
		ImGui::GetWindowDrawList()->AddRect(iconScreenPos, iconScreenPos + ImVec2(iconSize, iconSize), IM_COL32(200, 200, 200, 255), 4.0f, 0, 1.5f);

		DrawIconCentered(ImGui::GetWindowDrawList(), iconScreenPos, iconSize, GetIconForPath(path));

		ImGui::SetCursorPos(ImVec2(previewStart.x + (cellW - textW) * 0.5f, ImGui::GetCursorPosY()));
		ImGui::TextUnformatted(label.c_str());

		ImGui::EndDragDropSource();
	}
}

} // namespace GLEngine::Editor
