#pragma once

#include <GLRenderer/GUI/GUIWindow.h>

namespace GLEngine::Entity
{
class C_EntityManager;
}

namespace GLEngine::GLRenderer::GUI {
class C_EntitiesWindow
	: public C_Window
{
public:
	C_EntitiesWindow(GUID guid, std::shared_ptr<Entity::C_EntityManager>& world);
	virtual ~C_EntitiesWindow();

	virtual void Draw() const override;
private:
	std::shared_ptr<Entity::C_EntityManager> m_World;
};
}