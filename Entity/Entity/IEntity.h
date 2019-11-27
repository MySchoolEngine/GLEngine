#pragma once

#include <Entity/EntityApi.h>

#include <Core/GUID.h>
#include <Core/EventSystem/EventReciever.h>
#include <Entity/IComponent.h>

namespace GLEngine {
namespace Entity {

enum class E_ComponentType {
	Graphical,
	Camera,
	DebugGUI,
};

class ENTITY_API_EXPORT I_Entity : public Core::I_EventReciever {
public:
	// naive GUID version
	using EntityID = GUID;
	virtual ~I_Entity() = default;
	virtual EntityID GetID() const = 0;
	virtual T_ComponentPtr GetComponent(E_ComponentType type) const = 0;
	virtual std::string GetName() const = 0;

	virtual void Update() {};
	virtual void PostUpdate() {};

	template<E_ComponentType e,
		typename retType = ComponenetBase<e>::type,
		typename ret = std::shared_ptr<typename retType>>
	typename ret GetComponent() {
		return component_cast<e>(GetComponent(e));
	}
};

}
}