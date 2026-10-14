#include <RendererStdafx.h>

#include <Renderer/Components/Lights/LightFunctions.h>
#include <Renderer/DebugDraw.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace GLEngine::Renderer {

namespace {

// Extracts the pure rotation out of the world matrix, the same way C_Transformations::SetMatrix
// does (glm::decompose's rotation convention needs conjugating to match this codebase's convention
// of "rotate a local axis into world space").
glm::quat GetWorldRotation(const Entity::S_WorldTransformComponent& worldTransform)
{
	glm::vec3 scale, translation, skew;
	glm::vec4 perspective;
	glm::quat rotation;
	glm::decompose(worldTransform.world, scale, rotation, translation, skew, perspective);
	return glm::conjugate(rotation);
}

} // namespace

//=================================================================================
glm::vec3 GetWorldNormal(const Entity::S_WorldTransformComponent& worldTransform)
{
	return GetWorldRotation(worldTransform) * glm::vec3(0.f, 0.f, -1.f);
}

//=================================================================================
glm::vec3 GetWorldUpVector(const Entity::S_WorldTransformComponent& worldTransform)
{
	return GetWorldRotation(worldTransform) * glm::vec3(0.f, 1.f, 0.f);
}

//=================================================================================
Physics::Primitives::C_Frustum GetShadingFrustum(const S_PointLight&, const Entity::S_WorldTransformComponent& worldTransform)
{
	const glm::vec3 position(worldTransform.world[3]);
	return Physics::Primitives::C_Frustum(position, position, position, 1.f, 1.f, 1.f, 1.f);
}

//=================================================================================
Physics::Primitives::C_Frustum GetShadingFrustum(const S_AreaLight&, const Entity::S_WorldTransformComponent& worldTransform)
{
	const glm::vec3 position(worldTransform.world[3]);
	return Physics::Primitives::C_Frustum(position, GetWorldUpVector(worldTransform), GetWorldNormal(worldTransform), 0.1f, 50.f, 1.0f, 0.f);
}

//=================================================================================
Physics::Primitives::C_Frustum GetShadingFrustum(const S_SunLight&, const Entity::S_WorldTransformComponent&)
{
	return Physics::Primitives::C_Frustum(glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f), 0.f, 0.f, 0.f, 0.f);
}

//=================================================================================
Physics::Primitives::S_AABB GetAABB(const S_PointLight&, const Entity::S_WorldTransformComponent& worldTransform)
{
	Physics::Primitives::S_AABB aabb;
	aabb.Add(glm::vec3(worldTransform.world[3]));
	return aabb;
}

//=================================================================================
Physics::Primitives::S_AABB GetAABB(const S_AreaLight& light, const Entity::S_WorldTransformComponent& worldTransform)
{
	Physics::Primitives::S_AABB aabb;

	const auto dirY = GetWorldUpVector(worldTransform);
	const auto dirX = glm::cross(GetWorldNormal(worldTransform), dirY);

	const auto width  = std::sqrt(light.width / 2.0f);
	const auto height = std::sqrt(light.height / 2.0f);

	aabb.Add(+dirY * height + dirX * width);
	aabb.Add(+dirY * height - dirX * width);
	aabb.Add(-dirY * height + dirX * width);
	aabb.Add(-dirY * height - dirX * width);

	return aabb;
}

//=================================================================================
Physics::Primitives::S_AABB GetAABB(const S_SunLight&, const Entity::S_WorldTransformComponent&)
{
	return Physics::Primitives::S_AABB();
}

//=================================================================================
void DebugDraw(const S_AreaLight& light, const Entity::S_WorldTransformComponent& worldTransform, I_DebugDraw& dd)
{
	const auto upVector = GetWorldUpVector(worldTransform);
	const auto normal	= GetWorldNormal(worldTransform);
	const auto dirX		= glm::cross(normal, upVector);
	const auto width	= std::sqrt(light.width / 2.0f);
	const auto height	= std::sqrt(light.height / 2.0f);

	const auto pos = glm::vec3(worldTransform.world[3]);

	dd.DrawLine(pos, pos + normal, Colours::yellow);
	dd.DrawLine(pos + upVector * height + dirX * width, pos + upVector * height - dirX * width, Colours::yellow);
	dd.DrawLine(pos - upVector * height + dirX * width, pos - upVector * height - dirX * width, Colours::yellow);
	dd.DrawPoint(pos + upVector * height + dirX * width, Colours::green);
	dd.DrawLine(pos - upVector * height + dirX * width, pos + upVector * height + dirX * width, Colours::yellow);
	dd.DrawLine(pos - upVector * height - dirX * width, pos + upVector * height - dirX * width, Colours::yellow);
}

//=================================================================================
void DebugDraw(const S_SunLight& light, const Entity::S_WorldTransformComponent&, I_DebugDraw& dd)
{
	dd.DrawPoint({light.sunX, light.sunY, light.sunZ}, Colours::yellow);
	dd.DrawLine({0.f, 0.f, 0.f}, {light.sunX, light.sunY, light.sunZ}, Colours::yellow);
}

} // namespace GLEngine::Renderer
