#pragma once

#include <Entity/EntityApi.h>
#include <Entity/IEntity.h>

namespace GLEngine::Entity {

/**
 * This class should now do only the "dirty" work of updating components etc
 */
class C_BasicEntity : public I_Entity {
public:
	explicit C_BasicEntity(std::string name);
	explicit C_BasicEntity();
	virtual ~C_BasicEntity();

	virtual void Update() override;
	virtual void PostUpdate() override;

	//================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

	ENTITY_API_EXPORT void	 SetModelMatrix(const glm::mat4& modelMatrix);
	virtual const glm::mat4& GetModelMatrix() const override;

	virtual glm::vec3 GetPosition() const override;

	RTTR_ENABLE(I_Entity);

private:
	glm::mat4 m_ModelMatrix;
};

} // namespace GLEngine::Entity