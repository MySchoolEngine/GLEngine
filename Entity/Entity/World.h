#pragma once

#include <Entity/EntityApi.h>

#include <Core/EventSystem/EventReciever.h>
#include <Core/GUID.h>

#include <entt/entt.hpp>
#include <filesystem>
#include <unordered_map>

namespace GLEngine::Entity {

class C_Entity;

class ENTITY_API_EXPORT C_World : public Core::I_EventReceiver {
public:
	C_World()			= default;
	~C_World() override = default;

	C_Entity CreateEntity(std::string name);
	C_Entity CreateEntityWithGuid(const GUID& guid, std::string name);
	void	 DestroyEntity(entt::entity entity);
	void	 ClearLevel();

	[[nodiscard]] C_Entity FindByGuid(const GUID& id);
	[[nodiscard]] C_Entity FindByName(const std::string& name);

	[[nodiscard]] bool SetParent(entt::entity child, entt::entity newParent);
	void			   Detach(entt::entity child);

	[[nodiscard]] entt::registry& Registry() { return m_Registry; }

	void OnUpdate();

	void								SetFilename(const std::filesystem::path& filename);
	[[nodiscard]] std::filesystem::path GetFilename() const;

	void OnEvent(Core::I_Event& event) override {}

private:
	entt::registry						   m_Registry;
	std::unordered_map<GUID, entt::entity> m_GuidLookup;
	std::vector<entt::entity>			   m_PendingDestroy;
	std::filesystem::path				   m_Filename;
};

} // namespace GLEngine::Entity
