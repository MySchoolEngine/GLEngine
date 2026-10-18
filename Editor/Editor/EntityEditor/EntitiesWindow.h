#pragma once

#include <Editor/EditorApi.h>
#include <Editor/EntityEditor/EntityComponentsEditor.h>

#include <GUI/GUIWindow.h>

#include <Core/GUID.h>

#include <entt/entt.hpp>

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

	// Draws one tree node and recurses into its children via S_RelationshipComponent's
	// firstChild/next linked list (same traversal shape as C_WorldSerializer::Save's writeEntity
	// walk). Reparent/remove/create-child requests are queued into the Pending* members rather
	// than applied immediately, since acting on a drag-drop drop mid-traversal would mutate the
	// same firstChild/next lists the recursion is currently walking.
	void DrawEntityNode(entt::entity e) const;

	Entity::C_World*				 m_World;
	mutable GUID					 m_SelectedEntity;
	mutable C_EntityCompoenentEditor m_ComponentEditor;

	// Set by DrawEntityNode during the tree walk, applied once after it completes.
	mutable entt::entity m_PendingReparentChild	 = entt::null;
	mutable entt::entity m_PendingReparentTarget = entt::null;
	mutable entt::entity m_PendingRemove		 = entt::null;
	mutable entt::entity m_PendingCreateChildOf	 = entt::null;

	// Brief on-screen feedback for a rejected (cycle-forming) reparent drop.
	mutable std::string m_StatusMessage;
	mutable float		m_StatusMessageTimeLeft = 0.f;
};
} // namespace GLEngine::Editor
