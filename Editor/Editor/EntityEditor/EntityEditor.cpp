#include <EditorStdafx.h>

#include <Editor/EntityEditor/EntityEditor.h>

#include <GUI/FileDialogWindow.h>
#include <GUI/GUIManager.h>
#include <GUI/Menu/MenuItem.h>

#include <Entity/BasicEntity.h>

#include <Core/Filesystem/Paths.h>

#include <Utils/Serialization/XMLSerialize.h>
#include <Utils/Serialization/XMLDeserialize.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace GLEngine::Editor {

//=================================================================================
EntityEditor::EntityEditor(GUID guid, GUI::C_GUIManager& guiMGR)
	: GUI::C_Window(guid, "Entity editor")
	, m_File("File")
{
	m_File.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>(
		"New entity", [&]() { m_QueuedOperation = QueuedOperation::NewEntity; }, "Ctrl+N"));
	m_File.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>(
		"Open entity", [&]() { m_QueuedOperation = QueuedOperation::OpenEntity; }, "Ctrl+O"));
	m_File.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>(
		"Save entity", [&]() { m_QueuedOperation = QueuedOperation::SaveEntity; }, "Ctrl+S"));
	m_File.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>(
		"Save entity as", [&]() { m_QueuedOperation = QueuedOperation::SaveEntityAs; }, "Ctrl+Shift+S"));
	m_File.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Close entity", [&]() { m_QueuedOperation = QueuedOperation::CloseEntity; }));
	AddMenu(m_File);

	const auto entityDialogGUID = NextGUID();
	m_FileDialog				= new GUI::C_FileDialogWindow(
		   Core::Filesystem::entityFile.generic_string(), "Select file",
		   [&, entityDialogGUID](const std::filesystem::path& entityFile) {
			   m_FileDialogPath = entityFile;
			   m_FileDialog->SetVisible(false);
			   // guiMGR.DestroyWindow(entityDialogGUID);
		   },
		   entityDialogGUID, Core::Filesystem::entityFolder);

	guiMGR.AddCustomWindow(m_FileDialog);
}

//=================================================================================
void EntityEditor::DrawComponents() const
{
	ImGuiID dockspace_id = ImGui::GetID("EntitiesEditor");
	if (ImGui::DockBuilderGetNode(dockspace_id) == NULL)
	{
		ImGui::DockBuilderRemoveNode(dockspace_id);							   // Clear out existing layout
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
		// ImGui::DockBuilderSetNodeSize(dockspace_id, dockspace_size);
		ImGuiID leftNode, rightNode;
		ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.20f, &leftNode, &rightNode);

		ImGui::DockBuilderDockWindow("Properties", leftNode);
		ImGui::DockBuilderDockWindow("View", rightNode);
		ImGui::DockBuilderFinish(dockspace_id);
	}
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	// ImGui::SetNextWindowDockID(dockspace_id); Nice to have
	ImGui::Begin("Properties");

	m_ComponentEditor.Draw();
	ImGui::End();
	// ImGui::SetNextWindowDockID(dockspace_id); Nice to have
	ImGui::Begin("View");
	ImGui::Text("view");
	ImGui::End();
}

//=================================================================================
void EntityEditor::SetEntity(std::weak_ptr<Entity::I_Entity>& entity)
{
	m_Entity = entity.lock();
}

//=================================================================================
bool EntityEditor::UnsavedWork() const
{
	return m_Entity != nullptr && m_HasChanged;
}

//=================================================================================
void EntityEditor::Update()
{
	if (m_Entity)
		m_Entity->Update();
	if (m_QueuedOperation != QueuedOperation::None)
	{
		if (ImGui::BeginPopupModal("Save?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Save change to the opened entity?");

			ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
			if (ImGui::Button("Yes", button_size))
			{
				SaveEntity(m_Path);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No", button_size))
			{
				m_HasChanged = false; // < Discard work
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", button_size))
			{
				m_QueuedOperation = QueuedOperation::None;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Discard?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Discard unsaved changes to the entity?");

			ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
			if (ImGui::Button("Yes", button_size))
			{
				m_HasChanged = false; // < Discard work
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No", button_size))
			{
				m_QueuedOperation = QueuedOperation::None;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			ImGui::EndPopup();
		}

		if (UnsavedWork() && m_QueuedOperation != QueuedOperation::SaveEntity)
		{
			if (m_QueuedOperation == QueuedOperation::CloseEntity)
			{
				if (!ImGui::IsPopupOpen("Discard?"))
					ImGui::OpenPopup("Discard?");
			}
			else
			{
				if (!ImGui::IsPopupOpen("Save?"))
					ImGui::OpenPopup("Save?");
			}
		}
		else
		{
			switch (m_QueuedOperation)
			{
			case QueuedOperation::None:
				break;
			case QueuedOperation::SaveEntity:
				if (m_Path.empty() && m_FileDialogPath.has_value() == false)
				{
					if (!m_FileDialog->IsVisible())
					{
						m_FileDialog->SetTitle("Save entity");
						m_FileDialog->SetVisible(true);
					}
				}
				else
				{
					if (m_Path.empty() == false)
					{
						SaveEntity(m_Path);
					}
					if (m_FileDialogPath.has_value())
					{
						SaveEntity(m_FileDialogPath.value());
						m_FileDialogPath = {};
						m_FileDialog->SetVisible(false);
						m_QueuedOperation = QueuedOperation::None;
					}
				}
				break;
			case QueuedOperation::SaveEntityAs:
				if (m_FileDialogPath.has_value() == false)
				{
					if (!m_FileDialog->IsVisible())
					{
						m_FileDialog->SetTitle("Save entity");
						m_FileDialog->SetVisible(true);
					}
				}
				else
				{
					if (m_FileDialogPath.has_value())
					{
						SaveEntity(m_FileDialogPath.value());
						m_FileDialogPath = {};
						m_FileDialog->SetVisible(false);
						m_QueuedOperation = QueuedOperation::None;
					}
				}
				break;
			case QueuedOperation::NewEntity:
				m_Entity	 = std::make_shared<Entity::C_BasicEntity>();
				m_Path		 = "";
				m_HasChanged = true;
				m_ComponentEditor.SetEntity(m_Entity);
				m_QueuedOperation = QueuedOperation::None;
				break;
			case QueuedOperation::OpenEntity:
				if (m_FileDialogPath.has_value())
				{
					// TODO
					m_FileDialogPath = {};
					m_FileDialog->SetVisible(false);
					m_QueuedOperation = QueuedOperation::None;
				}
				else
				{
					if (!m_FileDialog->IsVisible())
					{
						m_FileDialog->SetTitle("Select entity");
						m_FileDialog->SetVisible(true);
					}
				}
				break;
			case QueuedOperation::CloseEntity:
				DiscardWork();
				m_QueuedOperation = QueuedOperation::None;
				break;
			default:
				break;
			}
		}
	}

	if (!m_IsVisible)
	{
		m_IsVisible = true;
		RequestDestroy();
	}
}

//=================================================================================
void EntityEditor::DiscardWork()
{
	m_Entity.reset();
	m_Path		 = "";
	m_HasChanged = false;
}

//=================================================================================
void EntityEditor::OpenEntity(const std::filesystem::path& path)
{
	GLE_ASSERT(UnsavedWork() == false, "Trying to open new entity when old one is not destroyed.");
	if (m_Entity)
	{
		DiscardWork();
	}

	m_Path		 = path;
	m_HasChanged = false;

	pugi::xml_document doc;

	pugi::xml_parse_result result;
	result = doc.load_file(path.c_str());
	if (!result.status == pugi::status_ok)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Can't open config file for entity name: {}", path);
		return;
	}

	Utils::C_XMLDeserializer ds;
	auto loadEntity = ds.Deserialize<std::shared_ptr<Entity::C_BasicEntity>>(doc);
	if (loadEntity.has_value() == false)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Failed loading entity file: {}", path);
		return;
	}
	m_Entity = loadEntity.value();
}

//=================================================================================
void EntityEditor::SaveEntity(const std::filesystem::path& path)
{
	Utils::C_XMLSerializer s;
	const auto			   str = s.Serialize(m_Entity);
	m_Path					   = path;
	str.save_file(path.c_str());
}

//=================================================================================
bool EntityEditor::CanDestroy() const
{
	return UnsavedWork() == false;
}

//=================================================================================
void EntityEditor::RequestDestroy()
{
	m_QueuedOperation = QueuedOperation::CloseEntity;
	GUI::C_Window::RequestDestroy();
}

} // namespace GLEngine::Editor