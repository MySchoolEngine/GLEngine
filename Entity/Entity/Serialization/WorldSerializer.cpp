#include <EntityStdafx.h>

#include <Entity/ComponentRegistry.h>
#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/Serialization/WorldSerializer.h>
#include <Entity/World.h>

#include <Utils/Serialization/XMLDeserialize.h>
#include <Utils/Serialization/XMLSerialize.h>

#include <pugixml.hpp>

#include <functional>
#include <unordered_map>

namespace GLEngine::Entity {

//=================================================================================
pugi::xml_document C_WorldSerializer::Save(C_World& world) const
{
	pugi::xml_document doc;
	auto			   worldNode		  = doc.append_child("World");
	worldNode.append_attribute("version") = 2;

	auto& registry = world.Registry();

	// Flat <Entity> list with an optional `parent` guid attribute (not nested
	// XML) - this is what lets Load's hierarchy-resolution pass below work.
	// The recursive walk below only controls write order (parents before
	// children), which keeps saved files diff-friendly; it is not load-bearing
	// for correctness.
	std::function<void(entt::entity)> writeEntity = [&](entt::entity e) {
		const auto& identity		  = registry.get<S_IdentityComponent>(e);
		auto		node			  = worldNode.append_child("Entity");
		node.append_attribute("guid") = identity.id.toString().c_str();
		node.append_attribute("name") = identity.name.c_str();

		const auto& rel = registry.get<S_RelationshipComponent>(e);
		if (rel.parent != entt::null)
			node.append_attribute("parent") = registry.get<S_IdentityComponent>(rel.parent).id.toString().c_str();

		for (const auto& entry : C_ComponentRegistry::Instance().Entries())
		{
			if (entry.serializable && entry.has(registry, e))
			{
				auto				   compNode = node.append_child(entry.name.c_str());
				Utils::C_XMLSerializer serializer;
				serializer.SerializeInto(entry.get(registry, e), compNode);
			}
		}

		for (auto child = rel.firstChild; child != entt::null; child = registry.get<S_RelationshipComponent>(child).next)
		{
			writeEntity(child);
		}
	};

	for (auto e : registry.view<S_RelationshipComponent>())
	{
		if (registry.get<S_RelationshipComponent>(e).parent == entt::null)
			writeEntity(e);
	}

	return doc;
}

//=================================================================================
void C_WorldSerializer::Load(const pugi::xml_document& document, C_World& world, Core::C_ResourceManager& resMng, Core::LoadingQuery& query, bool loadHandlesInstantly) const
{
	world.ClearLevel();

	auto&										  registry = world.Registry();
	std::unordered_map<std::string, entt::entity> guidToEntity;

	const auto worldNode = document.child("World");
	for (const auto& entityNode : worldNode.children("Entity"))
	{
		const std::string guidStr = entityNode.attribute("guid").as_string();
		const std::string name	  = entityNode.attribute("name").as_string();

		auto entity = world.CreateEntityWithGuid(GUID(guidStr), name);
		guidToEntity.emplace(guidStr, entity.Handle());

		for (const auto& compNode : entityNode.children())
		{
			const auto* entry = C_ComponentRegistry::Instance().FindByName(compNode.name());
			if (entry == nullptr)
			{
				CORE_LOG(E_Level::Warning, E_Context::Entity, "Unknown component '{}' on entity '{}', skipped.", compNode.name(), name);
				continue;
			}
			entry->emplaceDefault(registry, entity.Handle());
			rttr::variant			 instance = entry->get(registry, entity.Handle());
			Utils::C_XMLDeserializer deserializer(resMng, query, loadHandlesInstantly);
			deserializer.DeserializeInto(compNode, instance);
		}
	}

	for (const auto& entityNode : worldNode.children("Entity"))
	{
		const auto parentAttr = entityNode.attribute("parent");
		if (!parentAttr)
			continue;
		const auto childGuid  = entityNode.attribute("guid").as_string();
		const auto parentGuid = parentAttr.as_string();
		if (!world.SetParent(guidToEntity.at(childGuid), guidToEntity.at(parentGuid)))
			CORE_LOG(E_Level::Warning, E_Context::Entity, "Entity '{}' has a parent reference that would form a cycle; left unparented.", childGuid);
	}
}

} // namespace GLEngine::Entity
