#pragma once

#include <GUI/GUIWindow.h>

namespace GLEngine::Entity {
class C_EntityManager;
class ENTITY_API_EXPORT C_EntitiesWindow : public GUI::C_Window {
public:
	C_EntitiesWindow(GUID guid, std::shared_ptr<C_EntityManager>& world);
	virtual ~C_EntitiesWindow();

	virtual void Draw() const override;

private:
	std::shared_ptr<C_EntityManager> m_World;
};
} // namespace GLEngine::Entity