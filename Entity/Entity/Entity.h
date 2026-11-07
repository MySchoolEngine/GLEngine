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
	[[nodiscard]] C_World*	   World() const { return m_World; }

	// Asserts if T is already present - use AddOrReplace to add-or-overwrite instead.
	template <class T, class... Args> T&	  Add(Args&&... args);
	template <class T, class... Args> T&	  AddOrReplace(Args&&... args);
	template <class T> [[nodiscard]] T&		  Get();
	template <class T> [[nodiscard]] const T& Get() const;
	template <class T> [[nodiscard]] T*		  TryGet();
	template <class T> [[nodiscard]] bool	  Has() const;
	template <class T> void					  Remove();

	// Mutates T in place via func(s) (zero or more, same as entt's own patch), then fires the
	// registry's on_update<T> signal - e.g. the S_DirtyTransformTag machinery connected for
	// S_TransformComponent in C_World's constructor. Get() returns a mutable T& with no way to
	// fire that signal, which invites editing a component without the observers that depend on
	// it ever finding out - prefer this over `World()->Registry().patch<T>(Handle(), ...)`
	// when a C_Entity is already in hand.
	template <class T, class... Func> decltype(auto) Patch(Func&&... func);

	[[nodiscard]] GUID				 GetGuid() const;
	[[nodiscard]] const std::string& GetName() const;

	[[nodiscard]] bool operator==(const C_Entity& other) const { return m_World == other.m_World && m_Handle == other.m_Handle; }

private:
	C_World*	 m_World  = nullptr;
	entt::entity m_Handle = entt::null;
};

} // namespace GLEngine::Entity

#include <Entity/Entity.inl>
