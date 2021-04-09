#pragma once

#include <Entity/BasicEntity.h>
#include <Entity/Components/DebugGUIComponent.h>

namespace GLEngine::Entity {
class C_BasicEntity;
}

namespace GLEngine::Entity {

class C_EntityDebugComponent : public C_DebugGUIComponent {
public:
	explicit C_EntityDebugComponent(std::shared_ptr<I_Entity> entity);
	virtual ~C_EntityDebugComponent();

	virtual void DrawContents() override;

	virtual std::string_view GetDebugComponentName() const override;

private:
	std::weak_ptr<C_BasicEntity> m_Entity;
};

class C_GUIDebugBuilder : public I_ComponenetBuilder {
public:
	ENTITY_API_EXPORT virtual std::shared_ptr<I_Component> Build(const pugi::xml_node& node, std::shared_ptr<I_Entity> owner) override;
};

} // namespace GLEngine::Entity