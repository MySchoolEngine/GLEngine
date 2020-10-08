#include <RendererStdafx.h>

#include <Renderer/ICameraComponent.h>

namespace GLEngine::Renderer {

//=================================================================================
I_CameraComponent::I_CameraComponent(std::shared_ptr<Entity::I_Entity>& owner) 
	: Entity::I_Component(owner)
{}

//=================================================================================
I_CameraComponent::~I_CameraComponent() = default;

//=================================================================================
GLEngine::Physics::Primitives::S_Ray I_CameraComponent::GetRay(const glm::vec2& screenPos) const
{
	const float z = 1.0f;
	const glm::vec3 ray_nds = glm::vec3(screenPos.x, screenPos.y, z);
	const glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	glm::vec4 ray_eye = glm::inverse(GetProjectionMatrix()) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	glm::vec4 ray_wor = glm::inverse(GetViewMatrix()) * ray_eye;
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);

	const Physics::Primitives::S_Ray ray{ GetPosition(), glm::vec3(ray_wor) };

	return ray;
}

//=================================================================================
GLEngine::Physics::Primitives::S_AABB I_CameraComponent::GetAABB() const
{
	return Physics::Primitives::S_AABB();
}

//=================================================================================
GLEngine::Entity::E_ComponentType I_CameraComponent::GetType() const
{
	return Entity::E_ComponentType::Camera;
}

}