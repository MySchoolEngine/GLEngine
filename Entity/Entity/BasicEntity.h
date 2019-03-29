#pragma once

#include <Entity/IEntity.h>

namespace GLEngine {
namespace Entity {

class API_EXPORT C_BasicEntity : public I_Entity {
public:
	C_BasicEntity(std::string name);
	virtual ~C_BasicEntity();
	virtual std::shared_ptr<I_Component> GetComponent(E_ComponentType type) const override;
	virtual std::string GetName() const override;


	void AddComponent(std::shared_ptr<I_Component> component);
protected:
	std::string m_Name;
	std::map<E_ComponentType, std::shared_ptr<I_Component>>* m_Components;
};

}
}