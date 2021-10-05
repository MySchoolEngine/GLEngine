#pragma once

#include <Entity/EntityApi.h>
#include <Entity/IComponent.h>

#include <Core/EventSystem/EventReciever.h>
#include <Core/GUID.h>

#include <Utils/MapValueIterator.h>
#include <Utils/Range.h>

#include <rttr/type>

namespace GLEngine::Entity {

enum class E_ComponentType
{
	Graphical,
	Light,
	Camera,
	DebugGUI,
};

class ENTITY_API_EXPORT I_Entity : public Core::I_EventReciever {
protected:
	using T_ComponentsContainer = std::multimap<E_ComponentType, T_ComponentPtr>*;
	using T_ComponentIter		= std::remove_pointer<T_ComponentsContainer>::type::iterator;
	using T_ComponentConstIter	= std::remove_pointer<T_ComponentsContainer>::type::const_iterator;
	using T_ComponentRange		= Utils::Range<Utils::MapValueIterator<T_ComponentIter>>;

public:
	explicit I_Entity(std::string name);
	virtual ~I_Entity();

	// naive GUID version
	using EntityID = GUID;
	[[nodiscard]] EntityID				   GetID() const { return m_ID; }
	[[nodiscard]] virtual T_ComponentRange GetComponents(E_ComponentType type) const;
	[[nodiscard]] virtual glm::vec3		   GetPosition() const	  = 0;
	[[nodiscard]] virtual const glm::mat4& GetModelMatrix() const = 0;
	[[nodiscard]] const std::string&	   GetName() const { return m_Name; };

	virtual void Update(){};
	virtual void PostUpdate(){};

	Physics::Primitives::S_AABB GetAABB() const;

	void AddComponent(T_ComponentPtr component);

	template <E_ComponentType e, typename retType = typename ComponenetBase<e>::type, typename ret = std::shared_ptr<retType>> [[nodiscard]] ret GetComponent()
	{
		auto range = GetComponents(e);
		if (range.empty())
			return nullptr;
		auto& first = range.begin();
		return component_cast<e>(*first);
	}
	//=================================================================================
	[[nodiscard]] virtual T_ComponentIter begin();
	[[nodiscard]] virtual T_ComponentIter end();

	RTTR_ENABLE();
protected:
	EntityID			  m_ID;
	std::string			  m_Name;
	T_ComponentsContainer m_Components;
};

} // namespace GLEngine::Entity