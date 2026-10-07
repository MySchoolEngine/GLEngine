#pragma once

#include <Entity/EntityApi.h>

#include <Core/GUID.h>

#include <entt/entt.hpp>
#include <string>

namespace GLEngine::Entity {

class C_World;

class ENTITY_API_EXPORT C_Entity {
public:
	C_Entity() = default;
	C_Entity(C_World& world, entt::entity handle);

	[[nodiscard]] bool		   IsValid() const;
	[[nodiscard]] entt::entity Handle() const { return m_Handle; }

	template <class T, class... Args> T&	  Add(Args&&... args);
	template <class T> [[nodiscard]] T&		  Get();
	template <class T> [[nodiscard]] const T& Get() const;
	template <class T> [[nodiscard]] T*		  TryGet();
	template <class T> [[nodiscard]] bool	  Has() const;
	template <class T> void					  Remove();

	[[nodiscard]] GUID				 GetGuid() const;
	[[nodiscard]] const std::string& GetName() const;

	[[nodiscard]] bool operator==(const C_Entity& other) const { return m_World == other.m_World && m_Handle == other.m_Handle; }

private:
	C_World*	 m_World  = nullptr;
	entt::entity m_Handle = entt::null;
};

} // namespace GLEngine::Entity

#include <Entity/Entity.inl>
