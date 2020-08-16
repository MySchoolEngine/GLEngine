#pragma once

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

#include <Renderer/RendererApi.h>

#include <Physics/Primitives/Frustum.h>

namespace GLEngine {
namespace Renderer {
class RENDERER_API_EXPORT I_Light : public Entity::I_Component {
public:
	I_Light(std::shared_ptr<Entity::I_Entity> owner)
		: Entity::I_Component(owner) {}
	virtual ~I_Light() = default;

	virtual Entity::E_ComponentType GetType() const override
	{
		return Entity::E_ComponentType::Light;
	}

	virtual Physics::Primitives::C_Frustum GetShadingFrustum() const = 0;
};
}

template<>
class ComponenetBase<Entity::E_ComponentType::Light> {
public:
	using type = Renderer::I_Light;
};
}

