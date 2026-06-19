#pragma once

#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/RayGeneration/Generator.h>
#include <Renderer/RayCasting/RayGeneration/RenderWorkUnit.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureView.h>

#include <functional>

namespace GLEngine::Renderer {
class I_CameraComponent;
class I_TextureViewStorage;

// Renderer should do exactly what name says. Render image.
// In this state renderer is also integrator, which is suboptimal. I would also like to have
// renderer capable of progressive probe updates that should internally use path or other integrator
class RENDERER_API_EXPORT C_RayRenderer {
public:
	C_RayRenderer(const C_RayTraceScene& scene);
	~C_RayRenderer();

	struct AdditionalTargets {
		I_TextureViewStorage* rowHeatMap = nullptr;
		I_TextureViewStorage* normalsMap = nullptr;
		I_TextureViewStorage* uvMap = nullptr;
		[[nodiscard]] bool	  CheckTargets(const I_TextureViewStorage& mainTarget) const;
	};

	// @param generatorFactory  called with image dimensions; returns a generator that
	//                          yields S_RenderWorkUnit values in the desired traversal order.
	// @param storageMutex      optional; if present, writes to weightedImage are synchronized.
	void Render(I_CameraComponent&									   camera,
				I_TextureViewStorage&								   weightedImage,
				I_TextureViewStorage&								   storage,
				std::mutex*											   storageMutex,
				int													   numSamplesBefore,
				std::function<Generator<S_RenderWorkUnit>(glm::uvec2)> generatorFactory,
				AdditionalTargets									   additional = AdditionalTargets());

	[[nodiscard]] std::size_t GetProcessedPixels() const;

	void SetMaxPathDepth(const std::size_t depth) { m_MaxDepth = depth; }

	// if you are using mutex to sync data should call those two methods
	// under the lock
	[[nodiscard]] bool NewResultAvailable() const { return m_NewResultAvailable; }
	void			   SetResultConsumed() { m_NewResultAvailable = false; }

private:
	static void			   AddSample(const glm::ivec2 coord, C_TextureView view, const glm::vec3 sample);
	void				   UpdateView(const S_RenderWorkUnit& unit, const C_TextureView& source, C_TextureView& target, unsigned int numSamples);
	std::size_t			   m_ProcessedPixels;
	std::size_t			   m_MaxDepth;
	bool				   m_NewResultAvailable;
	const C_RayTraceScene& m_Scene;
};
} // namespace GLEngine::Renderer
