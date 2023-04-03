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

} // namespace GLEngine::VkRenderer
