#include <EntityStdafx.h>

#include <Entity/EntityManager.h>

#include <Entity/IEntity.h>
#include <Entity/BasicEntity.h>
#include <Entity/IComponent.h>
#include <Entity/Components/EntityDebugComponent.h>

#include <GLRenderer/Components/ComponentBuilderFactory.h>

#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Frustum.h>
#include <Physics/Primitives/Intersection.h>

#include <Utils/Parsing/MatrixParse.h>

#include <imgui.h>

#include <pugixml.hpp>


namespace GLEngine {
namespace Entity {

//=================================================================================
C_EntityManager::C_EntityManager()
	: m_Entities(new std::remove_pointer<decltype(m_Entities)>::type)
{

}

//=================================================================================
C_EntityManager::~C_EntityManager()
{
	delete m_Entities;
}

//=================================================================================
std::shared_ptr<I_Entity> C_EntityManager::GetEntity(GUID id) const
{
	if (id == INVALID_GUID) {
		return nullptr;
	}
	return *std::find_if(m_Entities->begin(), m_Entities->end(), [id](const std::shared_ptr<I_Entity>&entity) {
		return entity->GetID() == id;
	});
}

//=================================================================================
std::shared_ptr<I_Entity> C_EntityManager::GetEntity(const std::string& name) const
{
	const auto it = std::find_if(m_Entities->begin(), m_Entities->end(), [name](const std::shared_ptr<I_Entity>& entity) {
		return entity->GetName() == name;
		});
	if (it == m_Entities->end())
	{
		return nullptr;
	}
	return *it;
}

//=================================================================================
std::shared_ptr<I_Entity> C_EntityManager::GetOrCreateEntity(const std::string& name)
{
	auto entity = GetEntity(name);
	if (entity)
	{
		return entity;
	}
	entity = std::make_shared<C_BasicEntity>(name);
	entity->AddComponent(std::make_shared<C_EntityDebugComponent>(entity));
	AddEntity(entity);
	return entity;
}

//=================================================================================
std::vector<std::shared_ptr<I_Entity>> C_EntityManager::GetEntities(Physics::Primitives::C_Frustum frust) const
{
	return *m_Entities;
}

//=================================================================================
const std::vector<std::shared_ptr<I_Entity>>& C_EntityManager::GetEntities() const
{
	return *m_Entities;
}

//=================================================================================
void C_EntityManager::AddEntity(std::shared_ptr<I_Entity> entity)
{
	m_Entities->push_back(entity);
}

//=================================================================================
void C_EntityManager::OnUpdate()
{
	for (auto& entity : *m_Entities)
	{
		entity->Update();
	}

	for (auto& entity : *m_Entities)
	{
		entity->PostUpdate();
	}
}

//=================================================================================
Physics::Primitives::S_RayIntersection C_EntityManager::Select(const Physics::Primitives::S_Ray& ray)
{
	std::vector<Physics::Primitives::S_RayIntersection> intersects;
	intersects.reserve(m_Entities->size());
	for (auto& entity : *m_Entities)
	{
		const auto aabb = entity->GetAABB();
		const auto distance = aabb.IntersectImpl(ray);
		if(distance > 0)
		{
			using namespace Physics::Primitives;
			S_RayIntersection intersection;
			intersection.entityId = entity->GetID();
			intersection.distance = distance;
			intersection.intersectionPoint = ray.origin + ray.direction * intersection.distance;
			intersection.ray = ray;
			intersects.emplace_back(std::move(intersection));
		}
	}
 	if (!intersects.empty())
	{
		using namespace Physics::Primitives;
		const auto& nearest = std::min_element(intersects.begin(), intersects.end(), 
			[](const auto& it, const auto& smallest) {
				return it.distance < smallest.distance;
			});
		return *nearest;
	}

	{
		using namespace Physics::Primitives;
		constexpr S_Plane plane{ glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) , -1};
		S_RayIntersection intersection;
		intersection.entityId = INVALID_GUID;
		intersection.distance = plane.IntersectImpl(ray);
		intersection.intersectionPoint = ray.origin + ray.direction*intersection.distance;
		intersection.ray = ray;
		return intersection;
	}
}

//=================================================================================
bool C_EntityManager::LoadLevel(const std::string& name, std::unique_ptr<I_ComponentBuilderFactory> cbf)
{
	CORE_LOG(E_Level::Info, E_Context::Core, "Loading level: {}", name);
	delete m_Entities;
	m_Entities = new std::remove_pointer<decltype(m_Entities)>::type;
	pugi::xml_document doc;

	pugi::xml_parse_result result;
	result = doc.load_file(name.c_str());
	if (!result.status == pugi::status_ok) {
		CORE_LOG(E_Level::Error, E_Context::Core, "Can't open config file for level name: {}", name);
		return false;
	}

	auto worldNode = doc.child("World");
	if (!worldNode)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Invalid level name: {}", name);
		return false;
	}

	auto debugBuilder = cbf->GetFactory("debug");

	if (auto entitiesNode = worldNode.child("Entities"))
	{
		for (const auto& entityNode : entitiesNode.children("Entity"))
		{
			auto entity = std::make_shared<C_BasicEntity>(entityNode.attribute("name").value());
			entity->AddComponent(debugBuilder->Build(pugi::xml_node(), entity));
			this->AddEntity(entity);

			if (auto componentsNode = entityNode.child("Components"))
			{
				for (const auto& componentNode : componentsNode.children())
				{
					auto builder = cbf->GetFactory(componentNode.name());
					if (builder)
					{
						entity->AddComponent(builder->Build(componentNode, entity));
					}
				}
			}

			const auto translation = Utils::Parsing::C_MatrixParser::ParseTransformation(entityNode);
			const auto rotation = Utils::Parsing::C_MatrixParser::ParseRotations(entityNode);
			const auto scale = Utils::Parsing::C_MatrixParser::ParseScale(entityNode);
			entity->SetModelMatrix(translation * rotation * scale);
		}

		for (const auto& entityNode : entitiesNode.children("ExternEntity"))
		{
			auto entity = std::make_shared<C_BasicEntity>(entityNode.attribute("name").value());
			entity->AddComponent(debugBuilder->Build(pugi::xml_node(), entity));
			AddEntity(entity);

			cbf->ConstructFromFile(entity, entityNode);

			const auto translation = Utils::Parsing::C_MatrixParser::ParseTransformation(entityNode);
			const auto rotation = Utils::Parsing::C_MatrixParser::ParseRotations(entityNode);
			const auto scale = Utils::Parsing::C_MatrixParser::ParseScale(entityNode);
			entity->SetModelMatrix(translation * rotation * scale);
		}
	}

	return true;
}

}
}