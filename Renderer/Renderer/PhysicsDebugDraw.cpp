#include <RendererStdafx.h>

#include <Renderer/PhysicsDebugDraw.h>


namespace GLEngine::Renderer {

//=================================================================================
PhysicsDebugDraw::PhysicsDebugDraw(I_DebugDraw& dd)
	: m_dd(dd)
{
}

//=================================================================================
void PhysicsDebugDraw::DrawSpring(const glm::vec3& begin, const glm::vec3& end, const unsigned int numThreads)
{
	m_dd.DrawLine(begin, end);
}

} // namespace GLEngine::Renderer