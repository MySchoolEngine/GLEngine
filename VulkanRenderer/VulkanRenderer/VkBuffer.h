#pragma once

#include <Renderer/Descriptors/BufferDescriptor.h>

namespace GLEngine::VkRenderer {

class C_VkBuffer {
public:
	C_VkBuffer(const Renderer::BufferDescriptor& desc);
	C_VkBuffer(const C_VkBuffer&) = delete;

	VkBuffer& GetBuffer();

	void UploadData(const void* data, std::size_t size);

	const Renderer::BufferDescriptor& GetDesc() const;

private:
	Renderer::BufferDescriptor m_desc;
	VkBuffer				   m_Buffer;
	VkDeviceMemory			   m_Memory;
	void*					   m_MappedMemory;

	friend class C_VkResourceManager;
};
} // namespace GLEngine::VkRenderer
