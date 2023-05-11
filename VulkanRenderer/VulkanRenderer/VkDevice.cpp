#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/VkDevice.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkDevice::C_VkDevice()
{
	m_GPUResourceManager.Init(this);
}

//=================================================================================
C_VkDevice::~C_VkDevice() = default;

//=================================================================================
void C_VkDevice::Init(VkDevice_T* device, VkPhysicalDevice_T* gpu)
{
	m_Device = device;
	m_GPU	 = gpu;
}

//=================================================================================
bool C_VkDevice::AllocateTexture(Renderer::I_DeviceTexture& texture)
{
	return false;
}

//=================================================================================
void C_VkDevice::DestroyTexture(Renderer::I_DeviceTexture& texture)
{
}

//=================================================================================
Renderer::I_Device::T_TextureHandle C_VkDevice::CreateTextureHandle(const Renderer::TextureDescriptor& desc)
{
	return nullptr;
}

//=================================================================================
bool C_VkDevice::AllocateSampler(Renderer::I_TextureSampler2D& texture)
{
	return false;
}

//=================================================================================
void C_VkDevice::DestroySampler(Renderer::I_TextureSampler2D& texture)
{
}

//=================================================================================
std::size_t C_VkDevice::GetAllocatedMemory() const
{
	return 0;
}

//=================================================================================
bool C_VkDevice::CreateView(VkImageView& resultView, VkImage& image, VkFormat format)
{
	VkImageViewCreateInfo createInfo{};
	createInfo.sType						   = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image						   = image;
	createInfo.viewType						   = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format						   = format;
	createInfo.components.r					   = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g					   = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b					   = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a					   = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel   = 0;
	createInfo.subresourceRange.levelCount	   = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount	   = 1;

	if (const auto result = vkCreateImageView(m_Device, &createInfo, nullptr, &resultView) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create image view. {}", result);
		return false;
	}

	return true;
}

//=================================================================================
void C_VkDevice::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	const VkBufferCreateInfo bufferInfo{
		.sType		 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size		 = size,
		.usage		 = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};

	if (const auto result = vkCreateBuffer(GetVkDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to create buffer. {}", result);
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(GetVkDevice(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{
		.sType			 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize	 = memRequirements.size,
		.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties),
	};

	if (const auto result = vkAllocateMemory(GetVkDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to allocate buffer memory. {}", result);
	}

	vkBindBufferMemory(GetVkDevice(), buffer, bufferMemory, 0);
}

//=================================================================================
SwapChainSupportDetails C_VkDevice::QuerySwapChainSupport(VkSurfaceKHR surface)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_GPU, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_GPU, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_GPU, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_GPU, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_GPU, surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}

//=================================================================================
uint32_t C_VkDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_GPU, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

//=================================================================================
C_VkResourceManager& C_VkDevice::GetRM()
{
	return m_GPUResourceManager;
}

//=================================================================================
void C_VkDevice::CreateImage(glm::uvec2 dim, VkBufferUsageFlags usage, VkImage& textureImage, VkDeviceMemory& textureImageMemory)
{
	const VkImageCreateInfo imageInfo{
		.sType		   = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.flags = 0,
		.imageType	   = VK_IMAGE_TYPE_2D,
		.format = VK_FORMAT_R32G32B32A32_SFLOAT,
		.extent = { .width= dim.x, .height = dim.y, .depth = 1,},
		.mipLevels	   = 1,
		.arrayLayers   = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT, // no multisampling
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // discards pixels on first transition
	};

	if (auto result = vkCreateImage(GetVkDevice(), &imageInfo, nullptr, &textureImage) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to create image. {}", result);
		return;
	}
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(GetVkDevice(), textureImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType			  = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(GetVkDevice(), &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(GetVkDevice(), textureImage, textureImageMemory, 0);
}

} // namespace GLEngine::VkRenderer
