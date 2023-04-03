#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/VkResourceManager.h>
#include <VulkanRenderer/VkDevice.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkResourceManager::C_VkResourceManager()
	: m_device(nullptr)
{
}

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
		m_device->CreateBuffer(desc.size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer->m_Buffer, buffer->m_Memory);
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
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
Renderer::Handle<Renderer::Texture> C_VkResourceManager::createTexture(const Renderer::TextureDescriptor& desc)
{
	throw std::logic_error("The method or operation is not implemented.");
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
VkRenderer::C_VkBuffer* C_VkResourceManager::GetBuffer(const Renderer::Handle<Renderer::Buffer>& handle)
{
	return m_BufferPool.GetResource(handle);
}

}
