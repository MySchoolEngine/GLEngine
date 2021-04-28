#include <EditorStdAfx.h>

#include <Editor/Utils/MousePicking.h>
#include <Renderer/ICameraComponent.h>

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

}
