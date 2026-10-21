#pragma once

#include <GUI/GUIPart.h>

#include <Entity/Entity.h>

namespace GLEngine::Editor {
// Draws every registered component the selected entity has (generically, via
// C_ComponentRegistry's drawGUI/get entries - no per-type code needed) and lets the user add any
// userAddable-registered component by name. Replaces the old RTTR-derived-class type selector,
// which doesn't apply here: the new component structs aren't related by inheritance.
class C_EntityCompoenentEditor : public GUI::I_GUIPart {
public:
	C_EntityCompoenentEditor() = default;

	virtual bool Draw() const override;

	void SetEntity(Entity::C_Entity entity);

private:
	Entity::C_Entity m_Entity;
};
} // namespace GLEngine::Editor
