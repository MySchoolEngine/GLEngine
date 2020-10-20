#pragma once

#include <Entity/EntityApi.h>

#include <Core/GUID.h>

namespace GLEngine {

// forward declaration
namespace Physics::Primitives {
class C_Frustum;
struct S_Ray;
struct S_RayIntersection;
}

namespace Entity {

class I_Entity;
class I_ComponentBuilderFactory;

class ENTITY_API_EXPORT C_EntityManager {
public:
	C_EntityManager();
	~C_EntityManager();
	[[nodiscard]] std::shared_ptr<I_Entity> GetEntity(GUID id) const;
	[[nodiscard]] std::shared_ptr<I_Entity> GetEntity(const std::string& name) const;
	[[nodiscard]] std::shared_ptr<I_Entity> GetOrCreateEntity(const std::string& name);
	[[nodiscard]] std::vector<std::shared_ptr<I_Entity>> GetEntities(Physics::Primitives::C_Frustum frust) const;
	[[nodiscard]] const std::vector<std::shared_ptr<I_Entity>>& GetEntities() const;
	void AddEntity(std::shared_ptr<I_Entity> entity);
	void OnUpdate();
	[[nodiscard]] Physics::Primitives::S_RayIntersection Select(const Physics::Primitives::S_Ray& ray);

	[[nodiscard]] bool LoadLevel(const std::string& name, std::unique_ptr<I_ComponentBuilderFactory> cbf);
private:
	std::vector<std::shared_ptr<I_Entity>>* m_Entities;
};

}
}