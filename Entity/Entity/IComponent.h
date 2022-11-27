#pragma once

#include <Entity/EntityApi.h>

#include <GUI/Input/Transformations.h>

#include <Core/EventSystem/EventReciever.h>

#include <rttr/type>
#include <rttr/registration_friend.h>

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
class ENTITY_API_EXPORT I_Component : public Core::I_EventReciever {
public:
	explicit I_Component(std::shared_ptr<I_Entity> owner);
	virtual ~I_Component()								  = default;
	[[nodiscard]] virtual E_ComponentType GetType() const = 0;

	virtual void OnEvent(Core::I_Event& event) override {}

	virtual void Update(float dt){};
	virtual void PostUpdate(){};

	// draws inside of prepared window
	virtual void			   DebugDrawGUI(){};
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
	RTTR_ENABLE();
	RTTR_REGISTRATION_FRIEND;
};

//=================================================================================
class I_ComponenetBuilder {
public:
	virtual std::shared_ptr<I_Component> Build(const pugi::xml_node& node, std::shared_ptr<I_Entity> owner) = 0;
	RTTR_ENABLE();
};

//=================================================================================
class I_ComponentBuilderFactory {
public:
	[[nodiscard]] virtual std::unique_ptr<Entity::I_ComponenetBuilder> GetFactory(const std::string& name)													  = 0;
	virtual void													   ConstructFromFile(std::shared_ptr<I_Entity> entity, const std::filesystem::path& file) = 0;
};

} // namespace Entity
//=================================================================================
// I want this accessible all around the engine
using T_ComponentPtr = std::shared_ptr<Entity::I_Component>;

//=================================================================================
template <Entity::E_ComponentType e> class ComponenetBase {
};

template <Entity::E_ComponentType e, typename retType = typename ComponenetBase<e>::type, typename ret = std::shared_ptr<retType>> constexpr ret component_cast(T_ComponentPtr comp)
{
	return std::static_pointer_cast<retType>(comp);
}

} // namespace GLEngine
