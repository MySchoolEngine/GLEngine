#pragma once

#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
class I_TextureViewStorage;
class RENDERER_API_EXPORT C_ProbeRenderer {
public:
	C_ProbeRenderer(const C_RayTraceScene& scene, unsigned int samplesPerRender);
	~C_ProbeRenderer();

	void Render(I_TextureViewStorage& texture, const glm::vec3 probePosition, std::mutex* storageMutex);

	[[nodiscard]] bool NewResultAvailable() const { return m_NewResultAvailable; }
	void			   SetResultConsumed() { m_NewResultAvailable = false; }

	void ResetProbe() { m_Hysteresis = 0.f; }

private:
	unsigned int		   m_SamplesPerRender;
	float				   m_Hysteresis;
	const C_RayTraceScene& m_Scene;
	bool				   m_NewResultAvailable;

};
} // namespace GLEngine::Renderer
