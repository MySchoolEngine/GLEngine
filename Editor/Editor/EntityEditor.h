#pragma once

#include <Editor/EditorApi.h>

#include <GUI/GUIWindow.h>
#include <GUI/Menu/Menu.h>

#include <Entity/IEntity.h>

namespace GLEngine::Editor {
class EDITOR_API_EXPORT EntityEditor final : public GUI::C_Window {
public:
	EntityEditor(GUID guid, GUI::C_GUIManager& guiMGR);

	virtual void Update() override;
	void SetEntity(std::weak_ptr<Entity::I_Entity>& entity);

private:
	virtual void	   DrawComponents() const override;
	[[nodiscard]] bool UnsavedWork() const;

	virtual std::string GetTitle() const override;

	void DiscardWork();
	void SaveWorkWindow();
	void OpenEntityWindow();
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
	};


	bool							  m_HasChanged = false;
	QueuedOperation					  m_QueuedOperation = QueuedOperation::None;
	GUI::Menu::C_Menu				  m_File;
	std::shared_ptr<Entity::I_Entity> m_Entity;
	std::filesystem::path			  m_Path;
};
} // namespace GLEngine::Editor
