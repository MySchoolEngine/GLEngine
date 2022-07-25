#pragma once

#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {
class I_CameraComponent;
class I_TextureViewStorage;
class C_STDSampler;

class RENDERER_API_EXPORT C_RayRenderer {
public:
	C_RayRenderer(const C_RayTraceScene& scene);
	~C_RayRenderer();

	// @var storageMutex is optional, if present the writes to the weighted will be synchronized
	void Render(I_CameraComponent& camera, I_TextureViewStorage& weightedImage, I_TextureViewStorage& storage, std::mutex* storageMutex, int numSamplesBefore);

	[[nodiscard]] std::size_t GetProcessedPixels() const;

	void SetMaxPathDepth(std::size_t depth) { m_MaxDepth = depth; }

	// if you are using mutex to sync data should call those two methods
	// under the lock
	[[nodiscard]] bool NewResultAviable() const { return m_NewResultAviable; }
	void			   SetResultConsumed() { m_NewResultAviable = false; }

private:
	void							AddSample(const glm::ivec2 coord, C_TextureView view, const glm::vec3 sample);
	[[nodiscard]] Colours::T_Colour Li_LightSampling(const Physics::Primitives::S_Ray& ray, C_STDSampler& rnd);
	[[nodiscard]] Colours::T_Colour Li_Direct(const Physics::Primitives::S_Ray& ray, C_STDSampler& rnd);
	[[nodiscard]] Colours::T_Colour Li_PathTrace(Physics::Primitives::S_Ray ray, C_STDSampler& rnd, int currentDepth);
	[[nodiscard]] Colours::T_Colour PathTrace(Physics::Primitives::S_Ray ray, C_STDSampler& rnd);
	void							UpdateView(unsigned int sourceLine, unsigned int numLines, C_TextureView& source, C_TextureView& target, unsigned int numSamples);
	const C_RayTraceScene&			m_Scene;
	std::size_t						m_ProcessedPixels;
	std::size_t						m_MaxDepth;
	bool							m_NewResultAviable;
};
} // namespace GLEngine::Renderer
