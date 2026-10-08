#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Components/Lights/LightComponents.h>

#include <Physics/Primitives/AABB.h>
#include <Physics/Primitives/Frustum.h>

#include <Entity/Components/CoreComponents.h>

namespace GLEngine::Renderer {

class I_DebugDraw;

[[nodiscard]] RENDERER_API_EXPORT Physics::Primitives::C_Frustum GetShadingFrustum(const S_PointLight& light, const Entity::S_WorldTransformComponent& worldTransform);
[[nodiscard]] RENDERER_API_EXPORT Physics::Primitives::C_Frustum GetShadingFrustum(const S_AreaLight& light, const Entity::S_WorldTransformComponent& worldTransform);
[[nodiscard]] RENDERER_API_EXPORT Physics::Primitives::C_Frustum GetShadingFrustum(const S_SunLight& light, const Entity::S_WorldTransformComponent& worldTransform);

[[nodiscard]] RENDERER_API_EXPORT Physics::Primitives::S_AABB GetAABB(const S_PointLight& light, const Entity::S_WorldTransformComponent& worldTransform);
[[nodiscard]] RENDERER_API_EXPORT Physics::Primitives::S_AABB GetAABB(const S_AreaLight& light, const Entity::S_WorldTransformComponent& worldTransform);
[[nodiscard]] RENDERER_API_EXPORT Physics::Primitives::S_AABB GetAABB(const S_SunLight& light, const Entity::S_WorldTransformComponent& worldTransform);

RENDERER_API_EXPORT void DebugDraw(const S_AreaLight& light, const Entity::S_WorldTransformComponent& worldTransform, I_DebugDraw& dd);
RENDERER_API_EXPORT void DebugDraw(const S_SunLight& light, const Entity::S_WorldTransformComponent& worldTransform, I_DebugDraw& dd);

} // namespace GLEngine::Renderer
