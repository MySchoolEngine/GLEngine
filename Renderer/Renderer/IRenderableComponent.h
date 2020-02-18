#pragma once

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

namespace GLEngine {
namespace Renderer {
	class I_RenderableComponent : public Entity::I_Component {
	public:
		I_RenderableComponent(std::shared_ptr<Entity::I_Entity> owner) 
			: Entity::I_Component(owner) {}
		virtual ~I_RenderableComponent() = default;

		virtual void PerformDraw() const = 0;
		virtual Entity::E_ComponentType GetType() const override
		{
			return Entity::E_ComponentType::Graphical;
		}

	};
}

template<>
class ComponenetBase<Entity::E_ComponentType::Graphical> {
public:
	using type = Renderer::I_RenderableComponent;
};
}

