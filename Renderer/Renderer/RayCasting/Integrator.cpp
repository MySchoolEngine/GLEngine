#include <RendererStdafx.h>

#include <Renderer/RayCasting/Integrator.h>

namespace GLEngine::Renderer {

I_Integrator::I_Integrator(const C_RayTraceScene& scene)
	: m_Scene(scene)
{
}
} // namespace GLEngine::Renderer
