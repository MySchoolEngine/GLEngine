#pragma once

#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureView.h>

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
		[[nodiscard]] bool	  CheckTargets(const I_TextureViewStorage& mainTarget) const;
	};

	// @var storageMutex is optional, if present the writes to the weighted will be synchronized
	void Render(I_CameraComponent&	  camera,
				I_TextureViewStorage& weightedImage,
				I_TextureViewStorage& storage,
				std::mutex*			  storageMutex,
				int					  numSamplesBefore,
				AdditionalTargets	  additional = {nullptr});

	[[nodiscard]] std::size_t GetProcessedPixels() const;

	void SetMaxPathDepth(const std::size_t depth) { m_MaxDepth = depth; }

	// if you are using mutex to sync data should call those two methods
	// under the lock
	[[nodiscard]] bool NewResultAvailable() const { return m_NewResultAvailable; }
	void			   SetResultConsumed() { m_NewResultAvailable = false; }

private:
	static void			   AddSample(const glm::uvec2 coord, C_TextureView view, const glm::vec3 sample);
	void				   UpdateView(unsigned int sourceLine, unsigned int numLines, const C_TextureView& source, C_TextureView& target, unsigned int numSamples);
	std::size_t			   m_ProcessedPixels;
	std::size_t			   m_MaxDepth;
	bool				   m_NewResultAvailable;
	const C_RayTraceScene& m_Scene;
};
} // namespace GLEngine::Renderer
