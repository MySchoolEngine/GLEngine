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

//=================================================================================
void C_VkBuffer::UploadData(const void* data, std::size_t size)
{
	GLE_ASSERT(m_desc.usage == Renderer::E_ResourceUsage::Persistent, "Uploading data to non-persistent buffer");
	memcpy(m_MappedMemory, data, size);
}

//=================================================================================
const Renderer::BufferDescriptor& C_VkBuffer::GetDesc() const
{
	return m_desc;
}

}
