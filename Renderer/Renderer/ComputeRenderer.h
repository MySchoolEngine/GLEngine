#pragma once

#include <Renderer/RendererApi.h>
#include <Renderer/Resources/RenderResourceHandle.h>

namespace GLEngine::Renderer {

struct ComputeRenderCall {
	std::vector<Handle<Buffer>>	 Buffers;
	std::vector<Handle<Texture>> Textures; // compress to one array using std::variant
	glm::uvec3					 NumWorkgroups;
	Handle<ComputePipeline>		 PipelineHandle;
};

class I_ComputeRenderInterface {
public:
	I_ComputeRenderInterface()			= default;
	virtual ~I_ComputeRenderInterface() = default;

	// I need to draw, draw instanced etc.
	virtual void Render(const ComputeRenderCall& call) = 0;
};

class RENDERER_API_EXPORT C_ComputeRenderer {
public:
	void Draw(ComputeRenderCall&& call);
	void Clear();
	void Commit(I_ComputeRenderInterface& interface);

private:
	std::vector<ComputeRenderCall> m_DrawCalls;
};
} // namespace GLEngine::Renderer