#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Descriptors/PipelineDescriptor.h>
#include <Renderer/Resources/RenderResourceHandle.h>

namespace GLEngine::Renderer {
class RENDERER_API_EXPORT C_PSOCache final {
public:
	bool			 HasPipelineCached(const PipelineDescriptor& desc) const;
	void			 CachePipeline(const PipelineDescriptor& desc, Handle<Pipeline> handle);
	Handle<Pipeline> GetPipeline(const PipelineDescriptor& desc) const;

	void SerializeCache(const std::filesystem::path& path) const;
	void DeserializeCache(const std::filesystem::path& path);

private:
	std::map<std::size_t, Handle<Pipeline>> m_Cache;
};
} // namespace GLEngine::Renderer
