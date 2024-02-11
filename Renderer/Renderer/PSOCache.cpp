#include <RendererStdafx.h>

#include <Renderer/PSOCache.h>

namespace GLEngine::Renderer {

//=================================================================================
bool C_PSOCache::HasPipelineCached(const PipelineDescriptor& desc) const
{
	return m_Cache.find(std::hash<PipelineDescriptor>{}(desc)) != m_Cache.end();
}

//=================================================================================
void C_PSOCache::CachePipeline(const PipelineDescriptor& desc, Handle<Pipeline> handle)
{
	m_Cache[std::hash<PipelineDescriptor>{}(desc)] = handle;
}

//=================================================================================
Handle<Pipeline> C_PSOCache::GetPipeline(const PipelineDescriptor& desc) const
{
	auto it = m_Cache.find(std::hash<PipelineDescriptor>{}(desc));
	if (it == m_Cache.end())
		return {};
	return it->second;
}

}
