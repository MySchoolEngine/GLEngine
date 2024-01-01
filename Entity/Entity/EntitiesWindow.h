#pragma once

#include <GUI/GUIWindow.h>
#include <GUI/Input/TypeSelector.h>

namespace GLEngine::Entity {
class C_EntityManager;
class ENTITY_API_EXPORT C_EntitiesWindow : public GUI::C_Window {
public:
	C_EntitiesWindow(GUID guid, std::shared_ptr<C_EntityManager>& world);
	virtual ~C_EntitiesWindow();

	void SetWorld(std::shared_ptr<C_EntityManager> world);

	virtual bool Draw() const override;

private:
	std::weak_ptr<C_EntityManager> m_World;
	mutable GUID				   m_SelectedEntity;
	GUI::Input::C_TypeSelector	   m_EntityTypeSelector;
	GUI::Input::C_TypeSelector	   m_ComponentTypeSelector;
};
} // namespace GLEngine::Entity