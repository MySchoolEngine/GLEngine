#include <EntityStdafx.h>

#include <Entity/World.h>

#include <Entity/IEntity.h>

#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Frustum.h>
#include <Physics/Primitives/Intersection.h>

#include <imgui.h>


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

}
}