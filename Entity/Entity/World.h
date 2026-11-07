#pragma once

#include <Entity/EntityApi.h>

#include <Core/EventSystem/EventReciever.h>
#include <Core/GUID.h>

#include <entt/entt.hpp>
#include <filesystem>
#include <unordered_map>
#include <vector>

namespace GLEngine::Physics::Primitives {
struct S_Ray;
struct S_RayIntersection;
struct S_Sphere;
} // namespace GLEngine::Physics::Primitives

namespace GLEngine::Entity {

class C_Entity;

class ENTITY_API_EXPORT C_World : public Core::I_EventReceiver {
public:
	C_World();
	~C_World() override = default;

	C_Entity CreateEntity(std::string name);
	C_Entity CreateEntityWithGuid(const GUID& guid, std::string name);
	void	 DestroyEntity(entt::entity entity);
	void	 ClearLevel();

	[[nodiscard]] C_Entity FindByGuid(const GUID& id);
	[[nodiscard]] C_Entity FindByName(const std::string& name);
	C_Entity			   GetOrCreateEntity(const std::string& name);

	[[nodiscard]] bool SetParent(entt::entity child, entt::entity newParent);
	void			   Detach(entt::entity child);

	[[nodiscard]] entt::registry&		Registry() { return m_Registry; }
	[[nodiscard]] const entt::registry& Registry() const { return m_Registry; }

	void OnUpdate();

	[[nodiscard]] Physics::Primitives::S_RayIntersection Select(const Physics::Primitives::S_Ray& ray);

	// Calls func(C_Entity) for every entity whose world-space bounds overlap the given sphere
	// (an approximate test: each entity's S_LocalBounds is converted to its own bounding sphere
	// via S_AABB::GetSphere(), then tested sphere-vs-sphere - the same approximation
	// MainPassTechnique's frustum cull already uses). No freshness check is done - the caller is
	// responsible for having already run TransformSystem::Update (and whichever system populates
	// S_LocalBounds) this frame, same precondition as Select() above.
	template <class Func> void ForEachInSphere(const Physics::Primitives::S_Sphere& sphere, Func&& func);

	void								SetFilename(const std::filesystem::path& filename);
	[[nodiscard]] std::filesystem::path GetFilename() const;

	void OnEvent(Core::I_Event& event) override {}

private:
	// Keeps m_GuidLookup correct regardless of how an entity is destroyed - the proper
	// DestroyEntity()+OnUpdate() path, or a direct Registry().destroy()/clear() bypass (F-018).
	void OnIdentityComponentDestroyed(entt::registry& registry, entt::entity e);

	entt::registry						   m_Registry;
	std::unordered_map<GUID, entt::entity> m_GuidLookup;
	std::vector<entt::entity>			   m_PendingDestroy;
	std::filesystem::path				   m_Filename;
};

} // namespace GLEngine::Entity

#include <Entity/World.inl>
