#pragma once

namespace GLRenderer::Physics::Primitives {
struct S_Ray;
}

namespace GLEngine::Renderer {
class C_RayTraceScene;
}

namespace GLEngine::Renderer::RayTracing {
struct S_VisibilityTester {
	S_VisibilityTester(const glm::vec3& start, const glm::vec3& end);

	bool IsVisible(const C_RayTraceScene& scene, float eps = 1e-3f) const;

	Physics::Primitives::S_Ray GetRay() const;
	// would need something for medias and translucency
private:
	glm::vec3 m_Start;
	glm::vec3 m_End;
};
} // namespace GLEngine::Renderer::RayTracing