#pragma once

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

namespace GLEngine {
namespace Renderer {
	class I_DebugGUIComponent : public Entity::I_Component {
	public:
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

