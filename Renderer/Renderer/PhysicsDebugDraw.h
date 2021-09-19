#pragma once

#include <Renderer/RendererApi.h>
#include <Renderer/DebugDraw.h>

// Move to physics?
namespace GLEngine::Renderer {
class RENDERER_API_EXPORT PhysicsDebugDraw {
public:
	PhysicsDebugDraw(I_DebugDraw& dd);

	void DrawSpring(const glm::vec3& begin, const glm::vec3& end, const unsigned int numThreads);

private:
	I_DebugDraw& m_dd;
};
} // namespace GLEngine::Renderer
