#include <EditorStdafx.h>

#include <Editor/EditorColours.h>
#include <Editor/Editors/ImageEditor.h>
#include <Editor/Editors/MaterialPreview/MaterialPreviewWindow.h>
#include <Editor/Editors/ResourceManager/ResourceManagerWindow.h>
#include <Editor/Editors/TrimeshPreview/TrimeshPreviewWindow.h>

#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/Materials/MeshMaterialExtractor.h>
#include <Renderer/RayCasting/Geometry/TrimeshModel.h>
#include <Renderer/Textures/TextureResource.h>

#include <GUI/ImGuiLayer.h>

#include <Core/Resources/ResourceManager.h>

#include <IconsFontAwesome6.h>
#include <imgui.h>
#include <set>

#ifdef WIN32
	#include <windows.h>
#elif __linux__
	#include <sys/inotify.h>
	#include <sys/select.h>
	#include <unistd.h>
#endif

namespace {
const char* GetIconForPath(const std::filesystem::path& path)
{
	if (std::filesystem::is_directory(path))
		return ICON_FA_FOLDER;
	const auto ext = path.extension().string();

	auto& resMgr = GLEngine::Core::C_ResourceManager::Instance();
	if (resMgr.IsResourceType<GLEngine::Renderer::MeshResource>(path))
	{
		return ICON_FA_CUBE;
	}
	if (resMgr.IsResourceType<GLEngine::Renderer::C_TrimeshModel>(path))
	{
		return ICON_FA_BOXES_STACKED;
	}
	if (resMgr.IsResourceType<GLEngine::Renderer::TextureResource>(path))
	{
		return ICON_FA_FILE_IMAGE;
	}
	if (resMgr.IsResourceType<GLEngine::Renderer::MaterialResource>(path))
	{
		return ICON_FA_TROWEL_BRICKS;
	}

	return ICON_FA_FILE;
}

const char* GetTypeNameForPath(const std::filesystem::path& path)
{
	if (std::filesystem::is_directory(path))
		return "Folder";
	auto& resMgr = GLEngine::Core::C_ResourceManager::Instance();
	if (resMgr.IsResourceType<GLEngine::Renderer::MeshResource>(path))
		return "Mesh";
	if (resMgr.IsResourceType<GLEngine::Renderer::C_TrimeshModel>(path))
		return "Trimesh Model";
	if (resMgr.IsResourceType<GLEngine::Renderer::TextureResource>(path))
		return "Texture";
	if (resMgr.IsResourceType<GLEngine::Renderer::MaterialResource>(path))
		return "Material";
	return "File";
}

GLEngine::Colours::T_Colour GetColourForPath(const std::filesystem::path& path)
{
	if (std::filesystem::is_directory(path))
		return GLEngine::Colours::white;
	auto& resMgr = GLEngine::Core::C_ResourceManager::Instance();
	if (resMgr.IsResourceType<GLEngine::Renderer::MeshResource>(path))
		return GLEngine::Editor::Colours::Resources::Colour<GLEngine::Renderer::MeshResource>;
	if (resMgr.IsResourceType<GLEngine::Renderer::C_TrimeshModel>(path))
		return GLEngine::Editor::Colours::Resources::Colour<GLEngine::Renderer::C_TrimeshModel>;
	if (resMgr.IsResourceType<GLEngine::Renderer::TextureResource>(path))
		return GLEngine::Editor::Colours::Resources::Colour<GLEngine::Renderer::TextureResource>;
	if (resMgr.IsResourceType<GLEngine::Renderer::MaterialResource>(path))
		return GLEngine::Editor::Colours::Resources::Colour<GLEngine::Renderer::MaterialResource>;
	return GLEngine::Colours::white;
}

std::string FormatFileSize(std::uintmax_t bytes)
{
	if (bytes < 1024)
		return std::to_string(bytes) + " B";
	if (bytes < 1024 * 1024)
		return std::to_string(bytes / 1024) + " KB";
	return std::to_string(bytes / (1024 * 1024)) + " MB";
}

void DrawIconCentered(ImDrawList* drawList, ImVec2 rectMin, float rectSize, const char* iconStr, ImU32 colour)
{
	ImFont*		 font		= GLEngine::GUI::C_ImGuiLayer::GetLargeIconFont();
	const float	 fontSize	= font ? 32.0f : 16.0f;
	ImFont*		 renderFont = font ? font : ImGui::GetFont();
	const ImVec2 textSize	= renderFont->CalcTextSizeA(fontSize, FLT_MAX, 0.f, iconStr);
	const ImVec2 textPos(rectMin.x + (rectSize - textSize.x) * 0.5f, rectMin.y + (rectSize - textSize.y) * 0.5f);
	drawList->AddText(renderFont, fontSize, textPos, colour, iconStr);
}
} // namespace

// TODO First of all, for the drag and drop, I will need to define the IDs for draggable things inside of the editor.
// It will need to contain the information about the drug thing being resourced and its type. So it will be tied to the resource type.
// Secondly, I need to update the UI that is showing the resource inside of, for example, the component view or something like that.
// Next, I will need to tie the editor to the resource type. This will be done in editor Module anyway. It will tie the C++ type to the editor class.

namespace GLEngine::Editor {

//=================================================================================
C_ResourceManagerWindow::C_ResourceManagerWindow(GUID guid, GUI::C_GUIManager& guiMgr, std::filesystem::path rootPath, T_EventCallback eventCallback)
	: GUI::C_Window(guid, "Resource Manager")
	, m_RootPath(std::move(rootPath))
	, m_SelectedFolder(m_RootPath)
	, m_GUIManager(guiMgr)
	, m_EventCallback(std::move(eventCallback))
{
	StartWatcher(m_SelectedFolder);
}

//=================================================================================
C_ResourceManagerWindow::~C_ResourceManagerWindow()
{
	StopWatcher();
}

//=================================================================================
void C_ResourceManagerWindow::Update()
{
	if (m_ChangePending.exchange(false))
		m_ContentDirty = true;
}

//=================================================================================
void C_ResourceManagerWindow::StopWatcher() const
{
	m_WatcherRunning.store(false);
	if (m_WatcherThread.joinable())
		m_WatcherThread.join();
}

//=================================================================================
void C_ResourceManagerWindow::StartWatcher(const std::filesystem::path& path) const
{
	StopWatcher();
	m_WatcherRunning.store(true);
	m_WatcherThread = std::thread([this, path]() {
#ifdef WIN32
		HANDLE handle = FindFirstChangeNotificationW(path.wstring().c_str(), FALSE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE);
		if (handle == INVALID_HANDLE_VALUE)
			return;
		while (m_WatcherRunning.load())
		{
			const DWORD result = WaitForSingleObject(handle, 250);
			if (result == WAIT_OBJECT_0)
			{
				m_ChangePending.store(true);
				FindNextChangeNotification(handle);
			}
		}
		FindCloseChangeNotification(handle);
#elif __linux__
		const int fd = inotify_init1(IN_NONBLOCK);
		if (fd == -1)
			return;
		inotify_add_watch(fd, path.string().c_str(), IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_CLOSE_WRITE);
		char buf[4096];
		while (m_WatcherRunning.load())
		{
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(fd, &fds);
			timeval tv{0, 250000};
			if (select(fd + 1, &fds, nullptr, nullptr, &tv) > 0)
			{
				read(fd, buf, sizeof(buf));
				m_ChangePending.store(true);
			}
		}
		close(fd);
#endif
	});
}

//=================================================================================
void C_ResourceManagerWindow::DrawComponents() const
{
	const float totalWidth	  = ImGui::GetContentRegionAvail().x;
	const float folderPanelW  = totalWidth * 0.3f;
	const float contentPanelW = totalWidth - folderPanelW - ImGui::GetStyle().ItemSpacing.x;

	ImGui::BeginChild("##FolderTree", ImVec2(folderPanelW, 0) /*, ImGuiChildFlags_Border*/);

	ImGuiTreeNodeFlags rootFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (m_SelectedFolder == m_RootPath)
		rootFlags |= ImGuiTreeNodeFlags_Selected;
	const std::string rootLabel = std::string(ICON_FA_FOLDER) + "  " + m_RootPath.filename().string();
	const bool		  rootOpen	= ImGui::TreeNodeEx(rootLabel.c_str(), rootFlags);
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		OnFolderSelected(m_RootPath);
	if (rootOpen)
	{
		DrawFolderTree(m_RootPath);
		ImGui::TreePop();
	}

	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("##ContentPanel", ImVec2(contentPanelW, 0) /*, ImGuiChildFlags_Border*/);
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

		const bool		   isSelected = (entry.path() == m_SelectedFolder);
		ImGuiTreeNodeFlags flags	  = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
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
	// --- Filter bar ---
	static const char* s_TypeNames[] = {"All", "Folder", "Mesh", "Trimesh Model", "Texture", "Material", "File"};

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 120.f);
	ImGui::InputTextWithHint("##namefilter", ICON_FA_MAGNIFYING_GLASS " Search...", m_FilterName, sizeof(m_FilterName));
	ImGui::SameLine();
	ImGui::SetNextItemWidth(110.f);
	ImGui::Combo("##typefilter", &m_FilterTypeIndex, s_TypeNames, IM_ARRAYSIZE(s_TypeNames));

	const float filterBarH = ImGui::GetCursorPosY();

	// --- Reload disk contents if needed ---
	if (m_ContentDirty)
	{
		m_FolderContents.clear();
		std::error_code ec;
		for (const auto& entry : std::filesystem::directory_iterator(m_SelectedFolder, ec))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".meta")
				continue;
			m_FolderContents.push_back(entry.path());
		}
		m_ContentDirty = false;
	}

	// --- Build filtered view (no alloc on the hot path: just index pointers) ---
	const bool		hasNameFilter  = m_FilterName[0] != '\0';
	const char*		requiredType   = m_FilterTypeIndex > 0 ? s_TypeNames[m_FilterTypeIndex] : nullptr;
	const auto		containsCI	   = [](std::string_view hay, std::string_view needle) {
		return std::search(hay.begin(), hay.end(), needle.begin(), needle.end(),
						   [](char a, char b) { return std::tolower((unsigned char)a) == std::tolower((unsigned char)b); })
			   != hay.end();
	};

	std::vector<const std::filesystem::path*> filtered;
	filtered.reserve(m_FolderContents.size());
	for (const auto& path : m_FolderContents)
	{
		if (requiredType && std::strcmp(GetTypeNameForPath(path), requiredType) != 0)
			continue;
		if (hasNameFilter && !containsCI(path.filename().string(), m_FilterName))
			continue;
		filtered.push_back(&path);
	}

	// --- Grid ---
	constexpr float iconSize = 64.0f;
	constexpr float padding	 = 12.0f;
	constexpr float cellW	 = iconSize + padding;
	const float		cellH	 = iconSize + ImGui::GetTextLineHeightWithSpacing() + 4.0f;
	const int		numCols	 = std::max(1, static_cast<int>(ImGui::GetContentRegionAvail().x / cellW));

	for (int i = 0; i < static_cast<int>(filtered.size()); i++)
	{
		const int col = i % numCols;
		const int row = i / numCols;
		ImGui::SetCursorPos(ImVec2(col * cellW, filterBarH + row * cellH));
		DrawGridItem(*filtered[i], iconSize);
	}

	// Advance cursor past all rows so the child window scrolls correctly
	const int numRows = (static_cast<int>(filtered.size()) + numCols - 1) / numCols;
	ImGui::SetCursorPos(ImVec2(0.f, filterBarH + numRows * cellH));
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
	const auto& col = GetColourForPath(path);
	const ImU32 iconColour = IM_COL32(static_cast<int>(col.r * 255), static_cast<int>(col.g * 255), static_cast<int>(col.b * 255), 210);
	DrawIconCentered(ImGui::GetWindowDrawList(), rMin, iconSize, GetIconForPath(path), iconColour);

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(path.filename().string().c_str());
		ImGui::TextDisabled("%s", GetTypeNameForPath(path));
		if (!isDir)
		{
			std::error_code ec;
			const auto		sz = std::filesystem::file_size(path, ec);
			if (!ec)
				ImGui::TextDisabled("%s", FormatFileSize(sz).c_str());
		}
		ImGui::EndTooltip();
	}

	HandleResourceDragDrop(path, iconSize);
	HandleContextMenu(path);

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
	StartWatcher(path);
}

//=================================================================================
void C_ResourceManagerWindow::OnResourceDoubleClicked(const std::filesystem::path& path) const
{
	auto& resMgr = Core::C_ResourceManager::Instance();
	if (resMgr.IsResourceType<Renderer::TextureResource>(path))
	{
		auto handle = resMgr.LoadResource<Renderer::TextureResource>(path);

		auto* existing = dynamic_cast<C_ImageEditor*>(m_GUIManager.GetWindow(m_ImageEditorGUID));
		if (existing)
		{
			existing->OpenImage(handle);
		}
		else
		{
			m_ImageEditorGUID = NextGUID();
			auto* editor	  = new C_ImageEditor(m_ImageEditorGUID, m_GUIManager, m_EventCallback, handle);
			m_GUIManager.AddCustomWindow(editor);
			editor->SetVisible(true);
		}
		return;
	}

	// --- Trimesh → Trimesh Preview ---
	if (resMgr.IsResourceType<Renderer::C_TrimeshModel>(path))
	{
		auto handle = resMgr.LoadResource<Renderer::C_TrimeshModel>(path, /*isBlocking=*/true);
		if (!handle.IsReady())
			return;

		if (auto* win = static_cast<C_TrimeshPreviewWindow*>(m_GUIManager.GetWindow(m_TrimeshPreviewGUID)))
		{
			win->OpenModel(std::move(handle));
			win->SetVisible(true);
		}
		else
		{
			m_TrimeshPreviewGUID = NextGUID();
			auto* preview		 = new C_TrimeshPreviewWindow(m_TrimeshPreviewGUID, m_GUIManager);
			preview->OpenModel(std::move(handle));
			m_GUIManager.AddCustomWindow(preview);
			preview->SetVisible(true);
		}
		return;
	}

	// --- Material → Material Preview ---
	if (resMgr.IsResourceType<Renderer::MaterialResource>(path))
	{
		auto handle = resMgr.LoadResource<Renderer::MaterialResource>(path, /*isBlocking=*/true);
		if (!handle.IsReady())
			return;

		if (auto* win = static_cast<C_MaterialPreviewWindow*>(m_GUIManager.GetWindow(m_MaterialPreviewGUID)))
		{
			win->OpenMaterial(std::move(handle));
			win->SetVisible(true);
		}
		else
		{
			m_MaterialPreviewGUID = NextGUID();
			auto* preview		  = new C_MaterialPreviewWindow(m_MaterialPreviewGUID, m_GUIManager);
			preview->OpenMaterial(std::move(handle));
			m_GUIManager.AddCustomWindow(preview);
			preview->SetVisible(true);
		}
	}
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

		const auto& dragCol = GetColourForPath(path);
		const ImU32 dragIconColour = IM_COL32(static_cast<int>(dragCol.r * 255), static_cast<int>(dragCol.g * 255), static_cast<int>(dragCol.b * 255), 210);
		DrawIconCentered(ImGui::GetWindowDrawList(), iconScreenPos, iconSize, GetIconForPath(path), dragIconColour);

		ImGui::SetCursorPos(ImVec2(previewStart.x + (cellW - textW) * 0.5f, ImGui::GetCursorPosY()));
		ImGui::TextUnformatted(label.c_str());

		ImGui::EndDragDropSource();
	}
}

//=================================================================================
void C_ResourceManagerWindow::HandleContextMenu(const std::filesystem::path& path) const
{
	if (std::filesystem::is_directory(path))
		return;

	auto& resMgr = Core::C_ResourceManager::Instance();

	const bool hasEditor = resMgr.IsResourceType<Renderer::TextureResource>(path) || resMgr.IsResourceType<Renderer::C_TrimeshModel>(path)
						   || resMgr.IsResourceType<Renderer::MaterialResource>(path);
	const bool isMesh = resMgr.IsResourceType<Renderer::MeshResource>(path);

	if (!hasEditor && !isMesh)
		return;

	if (ImGui::BeginPopupContextItem())
	{
		if (hasEditor && ImGui::MenuItem("Open"))
			OnResourceDoubleClicked(path);

		if (isMesh && ImGui::MenuItem("Export Trimesh"))
			ExportTrimesh(path);

		if (isMesh && ImGui::MenuItem("Export Materials"))
			ExportMaterials(path, false);

		if (isMesh && ImGui::MenuItem("Force export Materials"))
			ExportMaterials(path, true);

		ImGui::EndPopup();
	}
}

//=================================================================================
void C_ResourceManagerWindow::ExportTrimesh(const std::filesystem::path& path) const
{
	auto& resMgr = Core::C_ResourceManager::Instance();

	// Load the base mesh first — this creates/populates the shared .meta file
	// (lada.obj and lada.tri share lada.meta) so the derived-resource build
	// flow can locate the base when we request the trimesh below.
	const auto meshHandle = resMgr.LoadResource<Renderer::MeshResource>(path, /*isBlocking=*/true);
	if (!meshHandle.IsReady())
		return;

	// Same directory, same stem, .tri extension
	auto triPath = path.parent_path() / path.stem();
	triPath.replace_extension(".tri");

	// ResourceManager will find lada.meta (already in-memory), load the base mesh,
	// call C_TrimeshModel::Build(), and save the .tri file.
	resMgr.LoadResource<Renderer::C_TrimeshModel>(triPath, /*isBlocking=*/false);
}

//=================================================================================
void C_ResourceManagerWindow::ExportMaterials(const std::filesystem::path& path, bool reexport) const
{
	auto& resMgr = Core::C_ResourceManager::Instance();

	// Load the base mesh first — this creates/populates the shared .meta file
	// (lada.obj and lada.tri share lada.meta) so the derived-resource build
	// flow can locate the base when we request the trimesh below.
	const auto meshHandle = resMgr.LoadResource<Renderer::MeshResource>(path, /*isBlocking=*/true);
	if (!meshHandle.IsReady())
		return;

	Renderer::ExtractMaterialsFromMesh(meshHandle.GetResource(), reexport);
}

} // namespace GLEngine::Editor
