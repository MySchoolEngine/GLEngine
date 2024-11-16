#pragma once

#include <Editor/EditorApi.h>
#include <Editor/EntityEditor/EntityComponentsEditor.h>

#include <GUI/GUIWindow.h>
#include <GUI/Input/TypeSelector.h>

namespace GLEngine::Entity {
class C_EntityManager;
}

namespace GLEngine::Editor {
class EDITOR_API_EXPORT C_EntitiesWindow : public GUI::C_Window {
public:
	C_EntitiesWindow(GUID guid, std::shared_ptr<Entity::C_EntityManager>& world);
	virtual ~C_EntitiesWindow();

	void SetWorld(std::shared_ptr<Entity::C_EntityManager> world);

	virtual bool Draw(GUI::C_GUIManager& guiMgr) const override;

private:
	std::weak_ptr<Entity::C_EntityManager> m_World;
	mutable GUID						   m_SelectedEntity;
	GUI::Input::C_TypeSelector			   m_EntityTypeSelector;
	mutable C_EntityCompoenentEditor	   m_ComponentEditor;
};
} // namespace GLEngine::Editor