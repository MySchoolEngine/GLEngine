#pragma once

#include <Entity/EntityApi.h>

#include <Core/EventSystem/EventReciever.h>
#include <Core/GUID.h>

#include <rttr/registration_friend.h>
#include <rttr/type>

namespace GLEngine {

// forward declaration
namespace Physics::Primitives {
class C_Frustum;
struct S_Ray;
struct S_RayIntersection;
} // namespace Physics::Primitives

namespace Entity {

class I_Entity;
class I_ComponentBuilderFactory;

class ENTITY_API_EXPORT C_EntityManager : public Core::I_EventReceiver {
public:
	C_EntityManager();
	~C_EntityManager();
	[[nodiscard]] std::shared_ptr<I_Entity>						GetEntity(GUID id) const;
	[[nodiscard]] std::shared_ptr<I_Entity>						GetEntity(const std::string& name) const;
	[[nodiscard]] std::vector<std::shared_ptr<I_Entity>>		GetEntities(Physics::Primitives::C_Frustum frust) const;
	[[nodiscard]] std::shared_ptr<I_Entity>						GetOrCreateEntity(const std::string& name);
	[[nodiscard]] const std::vector<std::shared_ptr<I_Entity>>& GetEntities() const;
	void														ClearLevel();
	void														AddEntity(std::shared_ptr<I_Entity> entity);
	void														OnUpdate();
	[[nodiscard]] Physics::Primitives::S_RayIntersection		Select(const Physics::Primitives::S_Ray& ray);

	void								SetFilename(const std::filesystem::path& filename);
	[[nodiscard]] std::filesystem::path GetFilename() const;

	virtual void OnEvent(Core::I_Event& event) override;

	RTTR_ENABLE();
	RTTR_REGISTRATION_FRIEND;
private:
	std::filesystem::path				   m_Filename;
	std::vector<std::shared_ptr<I_Entity>> m_Entities;
};

} // namespace Entity
} // namespace GLEngine