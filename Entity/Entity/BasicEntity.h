#pragma once

#include <Entity/IEntity.h>

namespace GLEngine {
namespace Entity {

class API_EXPORT C_BasicEntity : public I_Entity {
	using T_ComponentsContainer = std::map<E_ComponentType, std::shared_ptr<I_Component>>*;
public:
	C_BasicEntity(std::string name);
	virtual ~C_BasicEntity();
	virtual T_ComponentPtr GetComponent(E_ComponentType type) const override;
	virtual std::string GetName() const override;

	virtual void Update() override;
	virtual void PostUpdate() override;


	void AddComponent(std::shared_ptr<I_Component> component);

	//=================================================================================
	virtual EntityID GetID() const override;
	virtual void OnEvent(Core::I_Event& event) override;


	//=================================================================================
	using T_ComponentIter = std::remove_pointer<T_ComponentsContainer>::type::iterator;
	virtual T_ComponentIter begin();
	virtual T_ComponentIter end();

protected:
	EntityID m_ID;
	std::string m_Name;
	T_ComponentsContainer m_Components;
};

}
}