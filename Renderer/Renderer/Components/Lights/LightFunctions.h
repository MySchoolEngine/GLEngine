#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Components/Lights/LightComponents.h>

#include <Physics/Primitives/AABB.h>
#include <Physics/Primitives/Frustum.h>

#include <Entity/Components/CoreComponents.h>

namespace GLEngine::Renderer {

class I_DebugDraw;

// Direction a light's local -Z/+Y axis points in world space, given only its world transform -
// shared by area-light shading math and by RayTraceSceneBuilder, which needs the same vectors to
// place the ray-traced rectangle.
[[nodiscard]] RENDERER_API_EXPORT glm::vec3 GetWorldNormal(const Entity::S_WorldTransformComponent& worldTransform);
[[nodiscard]] RENDERER_API_EXPORT glm::vec3 GetWorldUpVector(const Entity::S_WorldTransformComponent& worldTransform);

[[nodiscard]] RENDERER_API_EXPORT Physics::Primitives::C_Frustum GetShadingFrustum(const S_PointLight& light, const Entity::S_WorldTransformComponent& worldTransform);
[[nodiscard]] RENDERER_API_EXPORT Physics::Primitives::C_Frustum GetShadingFrustum(const S_AreaLight& light, const Entity::S_WorldTransformComponent& worldTransform);
[[nodiscard]] RENDERER_API_EXPORT Physics::Primitives::C_Frustum GetShadingFrustum(const S_SunLight& light, const Entity::S_WorldTransformComponent& worldTransform);

[[nodiscard]] RENDERER_API_EXPORT Physics::Primitives::S_AABB GetAABB(const S_PointLight& light, const Entity::S_WorldTransformComponent& worldTransform);
[[nodiscard]] RENDERER_API_EXPORT Physics::Primitives::S_AABB GetAABB(const S_AreaLight& light, const Entity::S_WorldTransformComponent& worldTransform);
[[nodiscard]] RENDERER_API_EXPORT Physics::Primitives::S_AABB GetAABB(const S_SunLight& light, const Entity::S_WorldTransformComponent& worldTransform);

RENDERER_API_EXPORT void DebugDraw(const S_AreaLight& light, const Entity::S_WorldTransformComponent& worldTransform, I_DebugDraw& dd);
RENDERER_API_EXPORT void DebugDraw(const S_SunLight& light, const Entity::S_WorldTransformComponent& worldTransform, I_DebugDraw& dd);

} // namespace GLEngine::Renderer
