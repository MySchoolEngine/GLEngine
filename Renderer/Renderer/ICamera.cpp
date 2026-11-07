#include <RendererStdafx.h>

#include <Renderer/ICamera.h>

#include <Physics/Primitives/AABB.h>
#include <Physics/Primitives/Ray.h>

namespace GLEngine::Renderer {
RTTR_REGISTRATION
{
	rttr::registration::class_<I_Camera>("I_Camera");
}

//=================================================================================
I_Camera::I_Camera(const std::shared_ptr<Entity::I_Entity>& owner)
	: Entity::I_Component(owner)
{
}

//=================================================================================
I_Camera::~I_Camera() = default;

//=================================================================================
Physics::Primitives::S_Ray I_Camera::GetRay(const glm::vec2& screenPos) const
{
	const glm::vec4 ssFar = glm::vec4(screenPos.x, screenPos.y, 1.0, 1.0);
	const auto		mat	  = GetScreenToWorldMatrix();

	auto wFar = mat * ssFar;
	wFar /= wFar.w;

	const auto dir = glm::normalize(glm::vec3(wFar) - GetPosition());

	const Physics::Primitives::S_Ray ray{GetPosition(), dir};

	return ray;
}

//=================================================================================
Physics::Primitives::S_AABB I_Camera::GetAABB() const
{
	return Physics::Primitives::S_AABB();
}

//=================================================================================
GLEngine::Entity::E_ComponentType I_Camera::GetType() const
{
	return Entity::E_ComponentType::Camera;
}

} // namespace GLEngine::Renderer