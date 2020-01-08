#pragma once

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

namespace GLEngine {
namespace Renderer {
// @todo: Thats wrong, I_Component can anyway belong to single 
// entity
	class I_DebugGUIComponent : public Entity::I_Component {
	public:
		I_DebugGUIComponent(std::shared_ptr<Entity::I_Entity> owner) : Entity::I_Component(owner) {};
		virtual ~I_DebugGUIComponent() = default;
		virtual Entity::E_ComponentType GetType() const override
		{
			return Entity::E_ComponentType::DebugGUI;
		}

		virtual void Toggle() = 0;
	};
}

template<>
class ComponenetBase<Entity::E_ComponentType::DebugGUI> {
public:
	using type = Renderer::I_DebugGUIComponent;
};
}

