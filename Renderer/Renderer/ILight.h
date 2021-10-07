#pragma once

#include <Renderer/RendererApi.h>

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

#include <rttr/type>

namespace GLEngine::Physics::Primitives {
class C_Frustum;
}

namespace GLEngine {
namespace Renderer {
class RENDERER_API_EXPORT I_Light : public Entity::I_Component {
public:
	explicit I_Light(std::shared_ptr<Entity::I_Entity> owner)
		: Entity::I_Component(owner)
	{
	}
	virtual ~I_Light() = default;

	[[nodiscard]] virtual Entity::E_ComponentType GetType() const override { return Entity::E_ComponentType::Light; }

	virtual Physics::Primitives::C_Frustum GetShadingFrustum() const = 0;

	RTTR_ENABLE();
};
} // namespace Renderer

template <> class ComponenetBase<Entity::E_ComponentType::Light> {
public:
	using type = Renderer::I_Light;
};
} // namespace GLEngine
