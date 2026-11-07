#pragma once

#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>

namespace GLEngine::Entity {

template <class Func> void C_World::ForEachInSphere(const Physics::Primitives::S_Sphere& sphere, Func&& func)
{
	for (auto&& [e, bounds, worldTransform] : m_Registry.view<S_LocalBounds, S_WorldTransformComponent>().each())
	{
		if (sphere.IsColliding(bounds.aabb.getTransformedAABB(worldTransform.world).GetSphere()))
			func(C_Entity(*this, e));
	}
}

} // namespace GLEngine::Entity
