#pragma once

#include <Entity/EntityApi.h>

#include <Core/GUID.h>
#include <Core/EventSystem/EventReciever.h>
#include <Entity/IComponent.h>

namespace GLEngine {
namespace Entity {

enum class E_ComponentType {
	Graphical,
	Light,
	Camera,
	DebugGUI,
};

class ENTITY_API_EXPORT I_Entity : public Core::I_EventReciever {
private:
	using T_ComponentsContainer = std::map<E_ComponentType, T_ComponentPtr>*;
	using T_ComponentIter = std::remove_pointer<T_ComponentsContainer>::type::iterator;

public:
	I_Entity(std::string name);
	virtual ~I_Entity();

	// naive GUID version
	using EntityID = GUID;
	EntityID GetID() const { return m_ID; }
	virtual T_ComponentPtr GetComponent(E_ComponentType type) const;
	virtual glm::vec3 GetPosition() const = 0;
	const std::string& GetName() const { return m_Name; };

	virtual void Update() {};
	virtual void PostUpdate() {};

	Physics::Primitives::S_AABB GetAABB() const;

	void AddComponent(T_ComponentPtr component);

	template<E_ComponentType e,
		typename retType = ComponenetBase<e>::type,
		typename ret = std::shared_ptr<typename retType>>
	[[nodiscard]] typename ret GetComponent() {
		return component_cast<e>(GetComponent(e));
	}
	//=================================================================================
	virtual T_ComponentIter begin();
	virtual T_ComponentIter end();

protected:
	EntityID m_ID;
#pragma warning(push)
#pragma warning( disable : 4251)
	std::string m_Name;
#pragma warning(pop)
	T_ComponentsContainer m_Components;
};

}
}