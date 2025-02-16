#pragma once

#include <Renderer/RendererApi.h>

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

#include <rttr/registration>

namespace GLEngine {
namespace Renderer {
class RENDERER_API_EXPORT I_RenderableComponent : public Entity::I_Component {
public:
	explicit I_RenderableComponent(const std::shared_ptr<Entity::I_Entity>& owner)
		: Entity::I_Component(owner)
	{
	}
	~I_RenderableComponent() override = default;

	virtual void					PerformDraw() const = 0;
	Entity::E_ComponentType GetType() const override { return Entity::E_ComponentType::Graphical; }

	RTTR_ENABLE(Entity::I_Component);
};
} // namespace Renderer

template <> class ComponenetBase<Entity::E_ComponentType::Graphical> {
public:
	using type = Renderer::I_RenderableComponent;
};
} // namespace GLEngine