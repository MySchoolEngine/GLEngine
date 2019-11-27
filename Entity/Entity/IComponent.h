#pragma once

#include <Core/EventSystem/EventReciever.h>

namespace pugi {
class xml_node;
}

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

//=================================================================================
class I_ComponenetBuilder
{
public:
	virtual std::shared_ptr<I_Component> Build(const pugi::xml_node& node) = 0;
};

//=================================================================================
class I_ComponentBuilderFactory
{
public:
	virtual std::unique_ptr<Entity::I_ComponenetBuilder> GetFactory(const std::string& name) = 0;
};

}
//=================================================================================
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