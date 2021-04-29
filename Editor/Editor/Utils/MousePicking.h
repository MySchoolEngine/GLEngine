#pragma once

namespace GLEngine::Renderer {
class I_CameraComponent;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {
[[nodiscard]] float ScreenSpaceDistance(const glm::vec3& point, const glm::vec2& mousePosition, const Renderer::I_CameraComponent& camera);
[[nodiscard]] float ScreenSpaceDistanceToLine(const glm::vec3& a, const glm::vec3& b, const glm::vec2& mousePosition, const Renderer::I_CameraComponent& camera);
}