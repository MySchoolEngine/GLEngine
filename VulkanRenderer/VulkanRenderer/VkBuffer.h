#pragma once

#include <Renderer/Descriptors/BufferDescriptor.h>

namespace GLEngine::VkRenderer {

class C_VkBuffer {
public:
	C_VkBuffer(const Renderer::BufferDescriptor& desc);

	VkBuffer& GetBuffer();

private:
	const Renderer::BufferDescriptor& m_desc;
	VkBuffer						  m_Buffer;
	VkDeviceMemory					  m_Memory;

	friend class C_VkResourceManager;
};
}

