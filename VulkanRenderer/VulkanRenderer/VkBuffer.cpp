#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/VkBuffer.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkBuffer::C_VkBuffer(const Renderer::BufferDescriptor& desc)
	: m_desc(desc)
{
}

//=================================================================================
VkBuffer& C_VkBuffer::GetBuffer()
{
	return m_Buffer;
}

}
