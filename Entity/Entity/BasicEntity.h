#pragma once

#include <Entity/IEntity.h>

namespace GLEngine {
namespace Entity {

/**
 * This class should now do only the "dirty" work of updating components etc
 */
class ENTITY_API_EXPORT C_BasicEntity : public I_Entity {
public:
	C_BasicEntity(std::string name);
	virtual ~C_BasicEntity();

	virtual void Update() override;
	virtual void PostUpdate() override;

	//================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

	void SetModelMatrix(glm::mat4& modelMatrix);

private:
	glm::mat4 m_ModelMatrix;
};

}
}