#pragma once

#include <Entity/EntityApi.h>

#include <entt/entt.hpp>
#include <functional>
#include <rttr/type>
#include <string>
#include <string_view>
#include <vector>

namespace GLEngine::Entity {

struct S_ComponentRegistryEntry {
	std::string										   name;
	rttr::type										   type;
	std::function<bool(entt::registry&, entt::entity)> has;
	std::function<void(entt::registry&, entt::entity)> emplaceDefault;
	std::function<void(entt::registry&, entt::entity)> remove;
	// Returns a variant wrapping a std::reference_wrapper<T> (not a T by value) - RTTR's standard
	// idiom for aliasing an existing, live object so property get/set mutates it in place, rather
	// than a detached copy. rttr::variant cannot be constructed directly from an rttr::instance
	// (a static_assert in RTTR enforces this), so this returns rttr::variant, not rttr::instance.
	std::function<rttr::variant(entt::registry&, entt::entity)> get;
	bool														serializable = true;
	bool														userAddable	 = true;
	bool														drawGUI		 = true;
};

class ENTITY_API_EXPORT C_ComponentRegistry {
public:
	static C_ComponentRegistry& Instance();

	template <class T> void Register(std::string name, bool serializable = true, bool userAddable = true, bool drawGUI = true)
	{
		// S_ComponentRegistryEntry is constructed as a single aggregate-init expression, not via
		// a default-constructed local plus field assignment - rttr::type has no default
		// constructor, so a plain `S_ComponentRegistryEntry entry;` would implicitly delete the
		// struct's default constructor.
		m_Entries.push_back(S_ComponentRegistryEntry{
			.name = std::move(name),
			.type = rttr::type::get<T>(),
			.has  = [](entt::registry& registry, entt::entity e) { return registry.all_of<T>(e); },
			// emplace_or_replace, not emplace: entries like "Transform" describe components every
			// entity already carries (added unconditionally by C_World::CreateEntity*), so
			// WorldSerializer::Load re-applying a saved node for one must not assert on a component
			// that's already present.
			.emplaceDefault = [](entt::registry& registry, entt::entity e) { registry.emplace_or_replace<T>(e); },
			.remove			= [](entt::registry& registry, entt::entity e) { registry.remove<T>(e); },
			.get			= [](entt::registry& registry, entt::entity e) -> rttr::variant { return std::ref(registry.get<T>(e)); },
			.serializable	= serializable,
			.userAddable	= userAddable,
			.drawGUI		= drawGUI,
		});
	}

	[[nodiscard]] const std::vector<S_ComponentRegistryEntry>& Entries() const { return m_Entries; }
	[[nodiscard]] const S_ComponentRegistryEntry*			   FindByName(std::string_view name) const;

private:
	std::vector<S_ComponentRegistryEntry> m_Entries;
};

} // namespace GLEngine::Entity
