#pragma once

#include <GUI/DebugGUIComponent.h>

#include <Entity/BasicEntity.h>

namespace GLEngine::GUI {

class C_EntityDebugComponent : public C_DebugGUIComponent {
public:
	explicit C_EntityDebugComponent(std::shared_ptr<Entity::I_Entity> entity);
	virtual ~C_EntityDebugComponent() = default;

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