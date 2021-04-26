#pragma once

#include <GUI/GUIWindow.h>

namespace GLEngine::GUI {
class C_GUIManager;
}

namespace GLEngine::Entity {
class C_EntityManager;
class ENTITY_API_EXPORT C_EntitiesWindow : public GUI::C_Window {
public:
	C_EntitiesWindow(GUID guid, std::shared_ptr<C_EntityManager>& world, GUI::C_GUIManager& guiMGR);
	virtual ~C_EntitiesWindow();

	virtual void Draw() const override;

private:
	mutable std::shared_ptr<C_EntityManager> m_World;
	mutable std::weak_ptr<I_Entity>			 m_SelectedEntity;
	GUI::C_GUIManager&						 m_guiMGR; // gui manager refers to specific OS window, so we need to know where are we going to openg inner windows
};
} // namespace GLEngine::Entity