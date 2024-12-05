#pragma once

#include <Renderer/Descriptors/BufferDescriptor.h>
#include <Renderer/Descriptors/PipelineDescriptor.h>
#include <Renderer/Descriptors/TextureDescriptor.h>
#include <Renderer/Resources/RenderResourceHandle.h>

#include <filesystem>
#include <slot_map/slot_map.h>

namespace GLEngine::Renderer {
class ResourceManager {
public:
	ResourceManager()								  = default;
	virtual ~ResourceManager() noexcept				  = default;
	ResourceManager(const ResourceManager& other)	  = delete;
	ResourceManager(ResourceManager&& other) noexcept = delete;
	ResourceManager& operator=(const ResourceManager& other) = delete;
	ResourceManager& operator=(ResourceManager&& other) noexcept = delete;

	[[nodiscard]] virtual Handle<Shader>   createShader(const std::filesystem::path& path) = 0;
	virtual void						   destoryShader(Handle<Shader> handle)			   = 0;
	[[nodiscard]] virtual Handle<Pipeline> createPipeline(const PipelineDescriptor& desc) { return {}; }
	virtual void						   destoryPipeline(Handle<Pipeline> handle) {}
	[[nodiscard]] virtual Handle<Texture>  createTexture(const TextureDescriptor& desc)	  = 0;
	virtual void						   destoryTexture(Handle<Texture> handle)		  = 0;
	[[nodiscard]] virtual Handle<Buffer>   createBuffer(const BufferDescriptor& desc)	  = 0;
	virtual void						   destroyBuffer(const Handle<Buffer>& handle)	  = 0;
	[[nodiscard]] virtual Handle<Sampler>  createSampler(const SamplerDescriptor2D& desc) = 0;
	virtual void						   destroySampler(const Handle<Sampler>& handle)  = 0;
};

template <class Badge, class ConcreteResource> class ResourcePool final {
public:
	virtual ~ResourcePool() final
	{ /*
		GLE_ERROR(m_Map.empty());*/
	}
	ConcreteResource* GetResource(Handle<Badge> handle) { return m_Map.get(handle.m_Index); }

	template <class... Args> Handle<Badge> CreateNew(Args... args)
	{
		const dod::slot_map_key32<Badge> index = m_Map.emplace(std::forward<Args>(args)...);
		return Handle<Badge>(index);
	}

	void RemoveHandle(Handle<Badge> handle) { m_Map.erase(handle.m_Index); }

private:
	// question is, whether the badge should be generic one, or specific, specific one would make Handle API specific, but would avoid making mistakes
	dod::slot_map<ConcreteResource, dod::slot_map_key32<Badge>> m_Map;
};
} // namespace GLEngine::Renderer