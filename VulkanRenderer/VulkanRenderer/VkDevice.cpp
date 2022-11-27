#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/VkBuffer.h>
#include <VulkanRenderer/VkDevice.h>
#include <VulkanRenderer/VkTypeHelpers.h>

namespace GLEngine::VkRenderer {
//=================================================================================
C_VkDevice::C_VkDevice(VkDevice_T* device)
	: m_Device(device)
{
}

//=================================================================================
C_VkDevice::~C_VkDevice()
{
	vkDestroyDevice(m_Device, nullptr);
}

//=================================================================================
void C_VkDevice::DestroyTexture(Renderer::I_DeviceTexture& texture)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
bool C_VkDevice::AllocateTexture(Renderer::I_DeviceTexture& texture)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
C_VkDevice::T_TextureHandle C_VkDevice::CreateTextureHandle(const Renderer::TextureDescriptor& desc)
{
	return nullptr;
}

//=================================================================================
void C_VkDevice::DestroySampler(Renderer::I_TextureSampler2D& texture)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
bool C_VkDevice::AllocateSampler(Renderer::I_TextureSampler2D& texture)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
bool C_VkDevice::AllocateBuffer(Renderer::I_Buffer& buffer)
{
	auto* bufferVK = reinterpret_cast<C_VkBuffer*>(&buffer);
	GLE_ASSERT(bufferVK, "Wrong API buffer passed in");
	const auto& descriptor = buffer.GetDescriptor();
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_Device, bufferVK->m_Buffer, &memRequirements);

	return true;
}

//=================================================================================
void C_VkDevice::DestroyBuffer(Renderer::I_Buffer& buffer)
{
	auto* bufferVK = reinterpret_cast<C_VkBuffer*>(&buffer);
	GLE_ASSERT(bufferVK, "Wrong API buffer passed in");
	vkDestroyBuffer(m_Device, bufferVK->m_Buffer, nullptr);
}

//=================================================================================
Renderer::I_Device::T_BufferHandle C_VkDevice::CreateBufferHandle(const Renderer::BufferDescriptor& desc)
{
	auto buffer = std::make_shared<C_VkBuffer>(desc);

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType	   = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size		   = desc.size;
	bufferInfo.usage	   = GetVkBufferUsage(desc.type);
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &buffer->m_Buffer) != VK_SUCCESS)
	{
		return nullptr;
	}

	return buffer;
}

} // namespace GLEngine::VkRenderer
