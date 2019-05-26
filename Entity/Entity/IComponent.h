#pragma once

#include <Core/EventSystem/EventReciever.h>

namespace GLEngine {
namespace Entity {

enum class E_ComponentType;

//=================================================================================
class I_Component : public Core::I_EventReciever {
public:
	virtual ~I_Component() = default;
	virtual E_ComponentType GetType() const = 0;

	virtual void OnEvent(Core::I_Event& event) override {}

	virtual void Update() {};
	virtual void PostUpdate() {};

	// draws inside of prepared window
	virtual void DebugDrawGUI() {};
};

}
// I want this accessible all around the engine
using T_ComponentPtr = std::shared_ptr<Entity::I_Component>;

//=================================================================================
template<Entity::E_ComponentType e>
class ComponenetBase {
};

template<Entity::E_ComponentType e, 
	typename retType = ComponenetBase<e>::type,
	typename ret = std::shared_ptr<typename retType>>
constexpr ret  component_cast(T_ComponentPtr comp) {
	return std::static_pointer_cast<typename retType>(comp);
}

}