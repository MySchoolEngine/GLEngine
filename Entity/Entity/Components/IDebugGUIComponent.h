#pragma once

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

namespace GLEngine {
namespace Entity {
// @todo: Thats wrong, I_Component can anyway belong to single 
// entity
	class I_DebugGUIComponent : public I_Component {
	public:
		I_DebugGUIComponent(std::shared_ptr<I_Entity> owner) : I_Component(owner) {};
		virtual ~I_DebugGUIComponent() = default;
		virtual E_ComponentType GetType() const override
		{
			return E_ComponentType::DebugGUI;
		}

		virtual Physics::Primitives::S_AABB GetAABB() const override
		{
			return Physics::Primitives::S_AABB();
		}

		virtual void Toggle() = 0;
	};
}

template<>
class ComponenetBase<Entity::E_ComponentType::DebugGUI> {
public:
	using type = Entity::I_DebugGUIComponent;
};
}
