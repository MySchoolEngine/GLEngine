#pragma once

#include <Entity/IEntity.h>

namespace GLEngine {
namespace Entity {

class API_EXPORT C_BasicEntity : public I_Entity {
public:
	C_BasicEntity(std::string name);
	virtual ~C_BasicEntity();
	virtual T_ComponentPtr GetComponent(E_ComponentType type) const override;
	virtual std::string GetName() const override;


	void AddComponent(std::shared_ptr<I_Component> component);

	//=================================================================================
	virtual EntityID GetID() const override;

protected:
	EntityID m_ID;
	std::string m_Name;
	std::map<E_ComponentType, std::shared_ptr<I_Component>>* m_Components;
};

}
}