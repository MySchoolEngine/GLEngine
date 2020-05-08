#pragma once

#include <GLRenderer/GUI/GLDebugGUIComponent.h>

#include <Entity/BasicEntity.h>

namespace GLEngine::GLRenderer::GUI {

class C_GLEntityDebugComponent : public C_GLDebugGUIComponent {
public:
	explicit C_GLEntityDebugComponent(std::shared_ptr<Entity::I_Entity> entity);
	virtual ~C_GLEntityDebugComponent() = default;

	virtual void DrawContents() override;

private:
	std::weak_ptr<Entity::C_BasicEntity> m_Entity;
};

class C_GUIDebugBuilder : public Entity::I_ComponenetBuilder
{
public:
	virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;
};

}