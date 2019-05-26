#pragma once

#include <GLRenderer/GUI/GLDebugGUIComponent.h>

#include <Entity/BasicEntity.h>

namespace GLEngine {
namespace GLRenderer {
namespace GUI {

class C_GLEntityDebugComponent : public C_GLDebugGUIComponent {
public:
	C_GLEntityDebugComponent(std::shared_ptr<Entity::C_BasicEntity> entity);
	virtual ~C_GLEntityDebugComponent() = default;

	virtual void DrawContents() override;

private:
	std::shared_ptr<Entity::C_BasicEntity> m_Entity;
};

}
}
}