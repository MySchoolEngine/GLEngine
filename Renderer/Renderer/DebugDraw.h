#pragma once

namespace GLEngine::Physics::Primitives {
struct S_AABB;
}

namespace GLEngine::Renderer {

namespace Animation {
struct S_Joint;
class C_Skeleton;
} // namespace Animation

class I_DebugDraw {
public:
	virtual void DrawPoint(const glm::vec3& point, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f))				 = 0;
	virtual void DrawLine(const glm::vec3& pointA, const glm::vec3& pointB, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f))										 = 0;
	virtual void DrawLines(const std::vector<glm::vec4>& pairs, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f))													 = 0;
	virtual void DrawAABB(const Physics::Primitives::S_AABB& bbox, const glm::vec3& color = glm::vec3(0.0f, 0.0f, 0.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f)) = 0;

	virtual void DrawBone(const glm::vec3& position, const Animation::S_Joint& joint)		= 0;
	virtual void DrawSkeleton(const glm::vec3& root, const Animation::C_Skeleton& skeleton) = 0;
};
} // namespace GLEngine::Renderer
