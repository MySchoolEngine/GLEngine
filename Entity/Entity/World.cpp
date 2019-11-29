#include <EntityStdafx.h>

#include <Entity/World.h>

#include <Entity/IEntity.h>
#include <Entity/BasicEntity.h>
#include <Entity/IComponent.h>

#include <GLRenderer/Components/ComponentBuilderFactory.h>

#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Frustum.h>
#include <Physics/Primitives/Intersection.h>

#include <imgui.h>

#include <pugixml.hpp>


namespace GLEngine {
namespace Entity {

//=================================================================================
C_World::C_World()
	: m_Entities(new std::remove_pointer<decltype(m_Entities)>::type)
{

}

//=================================================================================
C_World::~C_World()
{
	delete m_Entities;
}

//=================================================================================
std::shared_ptr<I_Entity> C_World::GetEntity(GUID id) const
{
	if (id == INVALID_GUID) {
		return nullptr;
	}
	return *std::find_if(m_Entities->begin(), m_Entities->end(), [id](const std::shared_ptr<I_Entity>&entity) {
		return entity->GetID() == id;
	});
}

//=================================================================================
std::shared_ptr<I_Entity> C_World::GetEntity(const std::string& name) const
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
std::vector<std::shared_ptr<I_Entity>> C_World::GetEntities(Physics::Primitives::C_Frustum frust)
{
	return *m_Entities;
}

//=================================================================================
void C_World::AddEntity(std::shared_ptr<I_Entity> entity)
{
	m_Entities->push_back(entity);
}

//=================================================================================
void C_World::OnUpdate()
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
Physics::Primitives::S_RayIntersection C_World::Select(Physics::Primitives::S_Ray& ray)
{
	{
		using namespace Physics::Primitives;
		S_Plane plane;
		plane.noraml = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		plane.originOffset = -1;
		S_RayIntersection intersection;
		intersection.entityId = INVALID_GUID;
		intersection.distance = plane.Intersect(ray);
		intersection.intersectionPoint = ray.origin + ray.direction*intersection.distance;
		intersection.ray = ray;
		return intersection;
	}
}

//=================================================================================
bool C_World::LoadLevel(const std::string& name, std::unique_ptr<I_ComponentBuilderFactory> cbf)
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

	if (auto entitiesNode = worldNode.child("Entities"))
	{
		for (const auto& entityNode : entitiesNode.children("Entity"))
		{
			auto entity = std::make_shared<Entity::C_BasicEntity>(entityNode.attribute("name").value());
			this->AddEntity(entity);

			if (auto componentsNode = entityNode.child("Components"))
			{
				for (const auto& componentNode : componentsNode.children())
				{
					auto builder = cbf->GetFactory(componentNode.name());
					if (builder)
					{
						entity->AddComponent(builder->Build(componentNode));
					}
				}
			}
		}
	}

	return true;
}

}
}