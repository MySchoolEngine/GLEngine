#include <EditorStdafx.h>

#include <Editor/EntityEditor.h>

#include <GUI/FileDialogWindow.h>
#include <GUI/GUIManager.h>
#include <GUI/Menu/MenuItem.h>

#include <Entity/BasicEntity.h>

#include <Utils/Serialization/XMLSerialize.h>

#include <imgui.h>

namespace GLEngine::Editor {

//=================================================================================
EntityEditor::EntityEditor(GUID guid, GUI::C_GUIManager& guiMGR)
	: GUI::C_Window(guid, "Entity editor")
	, m_File("File")
{
	m_File.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>(
		"New entity", [&]() { m_QueuedOperation = QueuedOperation::NewEntity; }, "Ctrl+N"));
	m_File.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Open entity", [&]() {
		if (UnsavedWork())
		{
			m_QueuedOperation = QueuedOperation::OpenEntity;
			return;
		}
		OpenEntityWindow();
	}));
	m_File.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>(
		"Save entity", [&]() { SaveEntity(m_Path); }, "Ctrl+S"));
	m_File.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>(
		"Save entity as", [&]() { CORE_LOG(E_Level::Error, E_Context::Core, "New entity"); }, "Ctrl+Shift+S"));
	m_File.AddMenuItem(guiMGR.CreateMenuItem<GUI::Menu::C_MenuItem>("Close entity", [&]() { m_QueuedOperation = QueuedOperation::CloseEntity; }));
	AddMenu(m_File);
}

//=================================================================================
void EntityEditor::DrawComponents() const
{
	ImGuiID dockspace_id = ImGui::GetID("EntitiesEditor");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
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

		if (ImGui::BeginPopupModal("Discard?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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

		if (UnsavedWork())
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
			case QueuedOperation::NewEntity:
				m_Entity		  = std::make_shared<Entity::C_BasicEntity>();
				m_Path			  = "";
				m_HasChanged	  = true;
				m_QueuedOperation = QueuedOperation::None;
				break;
			case QueuedOperation::OpenEntity:
				break;
			case QueuedOperation::CloseEntity:
				DiscardWork();
				m_QueuedOperation = QueuedOperation::None;
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
void EntityEditor::OpenEntityWindow()
{
	// const auto levelSelectorGUID = NextGUID();
	// auto*	   levelSelectWindwo = new GUI::C_FileDialogWindow(
	// 	 ".ent", "Open entity",
	// 	 [&, levelSelectorGUID](const std::filesystem::path& entity) {
	// 		 CORE_LOG(E_Level::Error, E_Context::Core, "Open entity");
	// 		 OpenEntity(entity);
	// 		 guiMGR.DestroyWindow(levelSelectorGUID);
	// 	 },
	// 	 levelSelectorGUID, "./Entities");
	// guiMGR.AddCustomWindow(levelSelectWindwo);
	// levelSelectWindwo->SetVisible();
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
	m_Entity	 = std::make_shared<Entity::C_BasicEntity>(); // TODO!!
	m_HasChanged = false;
}

//=================================================================================
void EntityEditor::SaveWorkWindow()
{

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