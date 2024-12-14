#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/VkDevice.h>
#include <VulkanRenderer/VkResourceManager.h>
#include <VulkanRenderer/VkTypeHelpers.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkResourceManager::C_VkResourceManager()
	: m_device(nullptr)
{
}

//=================================================================================
C_VkResourceManager::~C_VkResourceManager() = default;

//=================================================================================
void C_VkResourceManager::Init(C_VkDevice* device)
{
	m_device = device;
}

//=================================================================================
Renderer::Handle<Renderer::Buffer> C_VkResourceManager::createBuffer(const Renderer::BufferDescriptor& desc)
{
	auto handle = m_BufferPool.CreateNew(desc);
	if (auto* buffer = m_BufferPool.GetResource(handle))
	{
		auto				  vkUsageBits = GetBufferType(desc.type);
		VkMemoryPropertyFlags resourceUsage{};
		if (desc.usage == Renderer::E_ResourceUsage::Immutable)
		{
			vkUsageBits |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			resourceUsage = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}
		if (desc.usage == Renderer::E_ResourceUsage::Persistent)
		{
			resourceUsage = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}
		m_device->CreateBuffer(desc.size, vkUsageBits, resourceUsage, buffer->m_Buffer, buffer->m_Memory);
		if (desc.usage == Renderer::E_ResourceUsage::Persistent)
		{
			// map memory
			vkMapMemory(m_device->GetVkDevice(), buffer->m_Memory, 0, desc.size, 0, &buffer->m_MappedMemory);
		}
	}
	return handle;
}

//=================================================================================
void C_VkResourceManager::destroyBuffer(const Renderer::Handle<Renderer::Buffer>& handle)
{
	auto* buffer = m_BufferPool.GetResource(handle);
	GLE_ASSERT(buffer, "Resource already destroyed");
	vkDestroyBuffer(m_device->GetVkDevice(), buffer->m_Buffer, nullptr);
	vkFreeMemory(m_device->GetVkDevice(), buffer->m_Memory, nullptr);
	m_BufferPool.RemoveHandle(handle);
}

//=================================================================================
void C_VkResourceManager::destoryTexture(Renderer::Handle<Renderer::Texture> handle)
{
	if (auto* texture = m_TexturePool.GetResource(handle))
	{
		vkDestroyImage(m_device->GetVkDevice(), texture->textureImage, nullptr);
		vkFreeMemory(m_device->GetVkDevice(), texture->textureImageMemory, nullptr);
		vkDestroyImageView(m_device->GetVkDevice(), texture->textureImageView, nullptr);
		m_TexturePool.RemoveHandle(handle);
	}
}

//=================================================================================
Renderer::Handle<Renderer::Texture> C_VkResourceManager::createTexture(const Renderer::TextureDescriptor& desc)
{
	auto handle = m_TexturePool.CreateNew(desc);
	if (auto* texture = m_TexturePool.GetResource(handle))
	{
		const VkImageCreateInfo imageInfo
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.flags = 0,
			.imageType = GetTextureType(desc.type),
			.format = GetTextureFormat(desc.format),
			.extent = { .width= static_cast<uint32_t>(desc.width), .height = static_cast<uint32_t>(desc.height), .depth = 1,},
			.mipLevels	   = 1,
			.arrayLayers   = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT, // no multisampling - from desc
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, // should be deduced
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // discards pixels on first transition
		};

		if (auto result = vkCreateImage(m_device->GetVkDevice(), &imageInfo, nullptr, &(texture->textureImage)) != VK_SUCCESS)
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "failed to create image. {}", result);
			return handle;
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_device->GetVkDevice(), texture->textureImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType			  = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize  = memRequirements.size;
		allocInfo.memoryTypeIndex = m_device->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(m_device->GetVkDevice(), &allocInfo, nullptr, &texture->textureImageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(m_device->GetVkDevice(), texture->textureImage, texture->textureImageMemory, 0);

		m_device->CreateView(texture->textureImageView, handle, *this); 
	}

	return handle;
}

//=================================================================================
Renderer::Handle<Renderer::Sampler> C_VkResourceManager::createSampler(const Renderer::SamplerDescriptor2D& desc)
{
	auto handle = m_SamplerPool.CreateNew(desc);
	if (auto* sampler = m_SamplerPool.GetResource(handle))
	{
		VkSamplerCreateInfo samplerInfo{
			.sType					 = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter				 = GetVkInternalFormat(desc.m_FilterMin),
			.minFilter				 = GetVkInternalFormat(desc.m_FilterMag),
			.mipmapMode				 = GetVkInternalMipMapFilter(desc.m_FilterMin), // what ever now
			.addressModeU			 = GetVkInternalFormat(desc.m_WrapS),
			.addressModeV			 = GetVkInternalFormat(desc.m_WrapT),
			.addressModeW			 = GetVkInternalFormat(desc.m_WrapU),
			.mipLodBias				 = 0.0f,
			.anisotropyEnable		 = VK_TRUE,
			.maxAnisotropy			 = 2,
			.compareEnable			 = VK_FALSE,
			.compareOp				 = VK_COMPARE_OP_ALWAYS,
			.minLod					 = 0.0f,
			.maxLod					 = 0.0f,
			.borderColor			 = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			.unnormalizedCoordinates = VK_FALSE,
		}; // otherwise vkGetPhysicalDeviceProperties

		if (const auto result = vkCreateSampler(m_device->GetVkDevice(), &samplerInfo, nullptr, &sampler->textureSampler) != VK_SUCCESS)
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "failed to create texture sampler. {}", result);
		}
	}
	return handle;
}

//=================================================================================
void C_VkResourceManager::destroySampler(const Renderer::Handle<Renderer::Sampler>& handle)
{
	if (auto* sampler = m_SamplerPool.GetResource(handle))
	{
		vkDestroySampler(m_device->GetVkDevice(), sampler->textureSampler, nullptr);
		m_SamplerPool.RemoveHandle(handle);
	}
}

//=================================================================================
void C_VkResourceManager::destoryShader(Renderer::Handle<Renderer::Shader> handle)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
Renderer::Handle<Renderer::Shader> C_VkResourceManager::createShader(const std::filesystem::path& path)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
C_VkBuffer* C_VkResourceManager::GetBuffer(const Renderer::Handle<Renderer::Buffer>& handle)
{
	return m_BufferPool.GetResource(handle);
}

//=================================================================================
C_VkTexture* C_VkResourceManager::GetTexture(const Renderer::Handle<Renderer::Texture>& handle)
{
	return m_TexturePool.GetResource(handle);
}

//=================================================================================
C_Pipeline* C_VkResourceManager::GetPipeline(const Renderer::Handle<Renderer::Pipeline>& handle)
{
	return m_PipelinePool.GetResource(handle);
}

//=================================================================================
VkRenderer::C_VkSampler* C_VkResourceManager::GetSampler(const Renderer::Handle<Renderer::Sampler>& handle)
{
	return m_SamplerPool.GetResource(handle);
}

//=================================================================================
Renderer::Handle<Renderer::Pipeline> C_VkResourceManager::createPipeline(const Renderer::PipelineDescriptor& desc)
{
	if (auto handle = m_PSOCache.GetPipeline(desc); handle.IsValid())
	{
		return handle;
	}
	// create and cache new pipeline
	auto handle = m_PipelinePool.CreateNew();
	if (C_Pipeline* pipeline = m_PipelinePool.GetResource(handle)) {
		pipeline->create(*m_device, desc);
	
		m_PSOCache.CachePipeline(desc, handle);
	}
	return handle;
}

//=================================================================================
void C_VkResourceManager::destoryPipeline(Renderer::Handle<Renderer::Pipeline> handle)
{
	// deliberately empty implementation. I don't want to delete cached pipelines
}

} // namespace GLEngine::VkRenderer
