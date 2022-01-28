#include <RendererStdafx.h>

#include <Renderer/ICameraComponent.h>

#include <Physics/Primitives/AABB.h>
#include <Physics/Primitives/Ray.h>

namespace GLEngine::Renderer {

//=================================================================================
I_CameraComponent::I_CameraComponent(std::shared_ptr<Entity::I_Entity> owner)
	: Entity::I_Component(owner)
{
}

//=================================================================================
I_CameraComponent::~I_CameraComponent() = default;

//=================================================================================
Physics::Primitives::S_Ray I_CameraComponent::GetRay(const glm::vec2& screenPos) const
{
	const glm::vec4 ssFar = glm::vec4(screenPos.x, screenPos.y, 1.0, 1.0);
	const auto		mat	  = GetScreenToworldMatrix();

	auto wFar = mat * ssFar;
	wFar /= wFar.w;

	const auto dir = glm::normalize(glm::vec3(wFar) - GetPosition());

	const Physics::Primitives::S_Ray ray{GetPosition(), dir};

	return ray;
}

//=================================================================================
Physics::Primitives::S_AABB I_CameraComponent::GetAABB() const
{
	return Physics::Primitives::S_AABB();
}

//=================================================================================
GLEngine::Entity::E_ComponentType I_CameraComponent::GetType() const
{
	return Entity::E_ComponentType::Camera;
}

} // namespace GLEngine::Renderer