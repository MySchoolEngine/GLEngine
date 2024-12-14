#pragma once

#include <Editor/EditorApi.h>
#include <Editor/EntityEditor/EntityComponentsEditor.h>

#include <GUI/GUIWindow.h>
#include <GUI/Menu/Menu.h>

#include <Entity/IEntity.h>

namespace GLEngine::GUI {
class C_FileDialogWindow;
}

namespace GLEngine::Editor {
class EDITOR_API_EXPORT EntityEditor final : public GUI::C_Window {
public:
	EntityEditor(GUID guid, GUI::C_GUIManager& guiMGR);

	virtual void Update() override;
	void		 SetEntity(std::weak_ptr<Entity::I_Entity>& entity);

private:
	virtual void	   DrawComponents() const override;
	[[nodiscard]] bool UnsavedWork() const;

	void DiscardWork();
	void OpenEntity(const std::filesystem::path& path);
	void SaveEntity(const std::filesystem::path& path);

	virtual bool CanDestroy() const override;

	virtual void RequestDestroy() override;


	enum class QueuedOperation
	{
		None,
		NewEntity,
		OpenEntity,
		CloseEntity,
		SaveEntity,
		SaveEntityAs,
	};


	bool								 m_HasChanged	   = false;
	QueuedOperation						 m_QueuedOperation = QueuedOperation::None;
	GUI::Menu::C_Menu					 m_File;
	std::shared_ptr<Entity::I_Entity>	 m_Entity;
	std::filesystem::path				 m_Path;
	GUI::C_FileDialogWindow*			 m_FileDialog;
	std::optional<std::filesystem::path> m_FileDialogPath;
	mutable C_EntityCompoenentEditor	 m_ComponentEditor;
};
} // namespace GLEngine::Editor
