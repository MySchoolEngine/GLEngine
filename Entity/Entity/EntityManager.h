#pragma once

#include <Entity/EntityApi.h>

#include <Core/GUID.h>

namespace GLEngine {

// forward declaration
namespace Physics {
namespace Primitives {
class C_Frustum;
struct S_Ray;
struct S_RayIntersection;
}
}

namespace Entity {

class I_Entity;
class I_ComponentBuilderFactory;

class ENTITY_API_EXPORT C_EntityManager {
public:
	C_EntityManager();
	~C_EntityManager();
	std::shared_ptr<I_Entity> GetEntity(GUID id) const;
	std::shared_ptr<I_Entity> GetEntity(const std::string& name) const;
	std::vector<std::shared_ptr<I_Entity>> GetEntities(Physics::Primitives::C_Frustum frust);
	void AddEntity(std::shared_ptr<I_Entity> entity);
	void OnUpdate();
	Physics::Primitives::S_RayIntersection Select(Physics::Primitives::S_Ray& ray);

	bool LoadLevel(const std::string& name, std::unique_ptr<I_ComponentBuilderFactory> cbf);
private:
	std::vector<std::shared_ptr<I_Entity>>* m_Entities;
};

}
}