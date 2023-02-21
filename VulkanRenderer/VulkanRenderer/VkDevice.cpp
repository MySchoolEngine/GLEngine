#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/VkDevice.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkDevice::C_VkDevice() = default;

//=================================================================================
C_VkDevice::~C_VkDevice() = default;

//=================================================================================
void C_VkDevice::Init(VkDevice_T* device)
{
	m_Device = device;
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

} // namespace GLEngine::VkRenderer
