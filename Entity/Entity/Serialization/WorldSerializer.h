#pragma once

#include <Entity/EntityApi.h>

namespace pugi {
class xml_document;
}

namespace GLEngine::Core {
class C_ResourceManager;
class LoadingQuery;
} // namespace GLEngine::Core

namespace GLEngine::Entity {

class C_World;

class ENTITY_API_EXPORT C_WorldSerializer {
public:
	[[nodiscard]] pugi::xml_document Save(C_World& world) const;
	void Load(const pugi::xml_document& document, C_World& world, Core::C_ResourceManager& resMng, Core::LoadingQuery& query, bool loadHandlesInstantly) const;
};

} // namespace GLEngine::Entity
