#pragma once

#include <Entity/EntityApi.h>

#include <GUI/Input/Transformations.h>

#include <Core/EventSystem/EventReciever.h>

#include <rttr/type>

namespace pugi {
class xml_node;
}

namespace GLEngine::Physics::Primitives {
struct S_AABB;
}

namespace GLEngine {
namespace Entity {

class I_Entity;

enum class E_ComponentType;

//=================================================================================
class ENTITY_API_EXPORT I_Component : public Core::I_EventReceiver {
public:
	explicit I_Component(const std::shared_ptr<I_Entity>& owner);
	~I_Component() override								  = default;
	[[nodiscard]] virtual E_ComponentType GetType() const = 0;

	void OnEvent(Core::I_Event& event) override {}

	virtual void Update() {};
	virtual void PostUpdate() {};

	// draws inside prepared window
	virtual void			   DebugDrawGUI() {};
	void					   DebugDrawComponentGUI();
	[[nodiscard]] virtual bool HasDebugDrawGUI() const = 0;
	// should return name used for component in debug
	[[nodiscard]] virtual std::string_view GetDebugComponentName() const = 0;
	// void SetModelMatrix(const glm::mat4& modelMatrix) { m_ModelMatrix = modelMatrix; };
	void						  SetComponentMatrix(const glm::mat4& componentMatrix) { m_Transformation.SetMatrix(componentMatrix); }
	[[nodiscard]] const glm::mat4 GetComponentModelMatrix() const;

	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const = 0;

	// only call on component without parent
	void SetParent(std::shared_ptr<I_Entity> owner);

protected:
	std::shared_ptr<I_Entity>	  GetOwner() const;
	GUI::Input::C_Transformations m_Transformation;

private:
	std::weak_ptr<I_Entity> m_Owner;
	RTTR_ENABLE()
	RTTR_REGISTRATION_FRIEND
};

} // namespace Entity
//=================================================================================
// I want this accessible all around the engine
using T_ComponentPtr = std::shared_ptr<Entity::I_Component>;

//=================================================================================
template <Entity::E_ComponentType e> class ComponenetBase {};

template <Entity::E_ComponentType e, typename retType = typename ComponenetBase<e>::type, typename ret = std::shared_ptr<retType>> constexpr ret component_cast(T_ComponentPtr comp)
{
	return std::static_pointer_cast<retType>(comp);
}

} // namespace GLEngine
