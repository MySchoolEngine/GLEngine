#pragma once

#include <Editor/EditorApi.h>
#include <Editor/EntityEditor/EntityComponentsEditor.h>

#include <GUI/GUIWindow.h>

#include <Core/GUID.h>

namespace GLEngine::Entity {
class C_World;
}

namespace GLEngine::Editor {
class EDITOR_API_EXPORT C_EntitiesWindow : public GUI::C_Window {
public:
	C_EntitiesWindow(GUID guid, Entity::C_World& world);
	virtual ~C_EntitiesWindow();

	void SetWorld(Entity::C_World& world);

	bool Draw(GUI::C_GUIManager& guiMgr) const override;

private:
	void SelectEntity(GUID id) const;

	Entity::C_World*				 m_World;
	mutable GUID					 m_SelectedEntity;
	mutable C_EntityCompoenentEditor m_ComponentEditor;
};
} // namespace GLEngine::Editor
