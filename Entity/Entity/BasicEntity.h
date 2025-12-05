#pragma once

#include <Entity/EntityApi.h>
#include <Entity/IEntity.h>

#include <memory>

namespace GLEngine::Entity {

/**
 * This class should now do only the "dirty" work of updating components etc
 */
class ENTITY_API_EXPORT C_BasicEntity : public I_Entity, public std::enable_shared_from_this<C_BasicEntity> {
public:
	explicit C_BasicEntity(std::string name);
	explicit C_BasicEntity();
	~C_BasicEntity() override;

	virtual void Update() override;
	virtual void PostUpdate() override;

	//================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

	void	 SetModelMatrix(const glm::mat4& modelMatrix);
	virtual const glm::mat4& GetModelMatrix() const override;

	virtual glm::vec3 GetPosition() const override;

	// Deserializer special method
	void AfterDeserialize();

	RTTR_ENABLE(I_Entity);

private:
	glm::mat4 m_ModelMatrix;
};

} // namespace GLEngine::Entity