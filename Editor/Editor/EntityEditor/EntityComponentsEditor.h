#pragma once

#include <GUI/GUIPart.h>
#include <GUI/Input/TypeSelector.h>

namespace GLEngine::Entity {
class I_Entity;
}

namespace GLEngine::Editor {
class C_EntityCompoenentEditor : public GUI::I_GUIPart {
public:
	C_EntityCompoenentEditor();

	virtual bool Draw() const override;

	void SetEntity(std::shared_ptr<Entity::I_Entity>);

private:
	std::weak_ptr<Entity::I_Entity> m_Entity;
	GUI::Input::C_TypeSelector		m_ComponentTypeSelector;
};
} // namespace GLEngine::Editor