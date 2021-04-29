#include <EditorStdAfx.h>

#include <Editor/Utils/MousePicking.h>
#include <Renderer/ICameraComponent.h>

#include <glm/glm.hpp>
#include <glm/gtx/closest_point.hpp>

namespace GLEngine::Editor {
//************************************
// Method:    ScreenSpaceDistance
// Returns:   float - distance of mouse and point in clip space ([x, y] x,y \in [-1;1]
// Qualifier:
// Parameter: const glm::vec3 & point
// Parameter: const glm::vec2 & mousePosition
// Parameter: const Renderer::I_CameraComponent & camera
//************************************
float ScreenSpaceDistance(const glm::vec3& point, const glm::vec2& mousePosition, const Renderer::I_CameraComponent& camera)
{
	const auto pointOnScreen = camera.GetViewProjectionMatrix() * glm::vec4(point, 1.f);
	const auto clipSpacePoint = glm::vec2(pointOnScreen.x, pointOnScreen.y) / pointOnScreen.w;

	return glm::distance(clipSpacePoint, mousePosition);
}

//=================================================================================
float ScreenSpaceDistanceToLine(const glm::vec3& a, const glm::vec3& b, const glm::vec2& mousePosition, const Renderer::I_CameraComponent& camera)
{
	const auto AinFrustum = camera.GetViewProjectionMatrix() * glm::vec4(a, 1.f);
	const auto BinFrustum = camera.GetViewProjectionMatrix() * glm::vec4(b, 1.f);

	const auto clipSpaceA = glm::vec2(AinFrustum.x, AinFrustum.y) / AinFrustum.w;
	const auto clipSpaceB = glm::vec2(BinFrustum.x, BinFrustum.y) / BinFrustum.w;

	return glm::distance(glm::closestPointOnLine(mousePosition, clipSpaceA, clipSpaceB), mousePosition);
}

}
