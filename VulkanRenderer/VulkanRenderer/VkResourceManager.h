#pragma once

#include <VulkanRenderer/Textures/VkSampler.h>
#include <VulkanRenderer/Textures/VkTexture.h>
#include <VulkanRenderer/VkBuffer.h>
#include <VulkanRenderer/Pipeline.h>

#include <Renderer/PSOCache.h>
#include <Renderer/Resources/ResourceManager.h>

namespace GLEngine::VkRenderer {
class C_VkDevice;
class C_VkTexture;
class C_VkSampler;

class C_VkResourceManager : public Renderer::ResourceManager {
public:
	C_VkResourceManager();
	~C_VkResourceManager() noexcept;

	void Init(C_VkDevice* device);

	[[nodiscard]] Renderer::Handle<Renderer::Shader>   createShader(const std::filesystem::path& path) override;
	void											   destoryShader(Renderer::Handle<Renderer::Shader> handle) override;
	[[nodiscard]] Renderer::Handle<Renderer::Texture>  createTexture(const Renderer::TextureDescriptor& desc) override;
	void											   destoryTexture(Renderer::Handle<Renderer::Texture> handle) override;
	[[nodiscard]] Renderer::Handle<Renderer::Buffer>   createBuffer(const Renderer::BufferDescriptor& desc) override;
	void											   destroyBuffer(const Renderer::Handle<Renderer::Buffer>& handle) override;
	[[nodiscard]] Renderer::Handle<Renderer::Sampler>  createSampler(const Renderer::SamplerDescriptor2D& desc) override;
	void											   destroySampler(const Renderer::Handle<Renderer::Sampler>& handle) override;
	[[nodiscard]] Renderer::Handle<Renderer::Pipeline> createPipeline(const Renderer::PipelineDescriptor& desc) override;
	void											   destoryPipeline(Renderer::Handle<Renderer::Pipeline> handle) override;

	C_VkBuffer*	 GetBuffer(const Renderer::Handle<Renderer::Buffer>& handle);
	C_VkTexture* GetTexture(const Renderer::Handle<Renderer::Texture>& handle);
	C_VkSampler* GetSampler(const Renderer::Handle<Renderer::Sampler>& handle);
	C_Pipeline*	 GetPipeline(const Renderer::Handle<Renderer::Pipeline>& handle);

private:
	Renderer::ResourcePool<Renderer::Buffer, C_VkBuffer>   m_BufferPool;
	Renderer::ResourcePool<Renderer::Texture, C_VkTexture> m_TexturePool;
	Renderer::ResourcePool<Renderer::Sampler, C_VkSampler> m_SamplerPool;
	Renderer::ResourcePool<Renderer::Pipeline, C_Pipeline> m_PipelinePool;
	Renderer::C_PSOCache								   m_PSOCache;
	C_VkDevice*											   m_device;
};
} // namespace GLEngine::VkRenderer
