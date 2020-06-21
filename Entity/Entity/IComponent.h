#pragma once

#include <Entity/EntityApi.h>

#include <Core/EventSystem/EventReciever.h>

namespace pugi {
class xml_node;
}

namespace GLEngine {
namespace Entity {

class I_Entity;

enum class E_ComponentType;

//=================================================================================
class ENTITY_API_EXPORT I_Component : public Core::I_EventReciever {
public:
	I_Component(std::shared_ptr<I_Entity> owner);
	virtual ~I_Component() = default;
	[[nodiscard]] virtual E_ComponentType GetType() const = 0;

	virtual void OnEvent(Core::I_Event& event) override {}

	virtual void Update() {};
	virtual void PostUpdate() {};

	// draws inside of prepared window
	virtual void DebugDrawGUI() {};
	//void SetModelMatrix(const glm::mat4& modelMatrix) { m_ModelMatrix = modelMatrix; };
	void SetComponentMatrix(const glm::mat4& componentMatrix) { m_ComponentMatrix = componentMatrix; }
	[[nodiscard]] const glm::mat4& GetComponentModelMatrix() const;

protected:
	std::shared_ptr<I_Entity> GetOwner() const;

#pragma warning(push)
#pragma warning( disable : 4251)
	glm::mat4 m_ComponentMatrix;
private:
	std::weak_ptr<I_Entity> m_Owner;
#pragma warning(pop)
};

//=================================================================================
class I_ComponenetBuilder
{
public:
	virtual std::shared_ptr<I_Component> Build(const pugi::xml_node& node, std::shared_ptr<I_Entity> owner) = 0;
};

//=================================================================================
class I_ComponentBuilderFactory
{
public:
	virtual std::unique_ptr<Entity::I_ComponenetBuilder> GetFactory(const std::string& name) = 0;
};

}
//=================================================================================
// I want this accessible all around the engine
using T_ComponentPtr = std::shared_ptr<Entity::I_Component>;

//=================================================================================
template<Entity::E_ComponentType e>
class ComponenetBase {
};

template<Entity::E_ComponentType e, 
	typename retType = ComponenetBase<e>::type,
	typename ret = std::shared_ptr<typename retType>>
constexpr ret  component_cast(T_ComponentPtr comp) {
	return std::static_pointer_cast<typename retType>(comp);
}

}