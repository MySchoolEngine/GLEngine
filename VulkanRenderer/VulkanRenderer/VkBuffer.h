#pragma once

#include <Renderer/Buffer.h>

namespace GLEngine::VkRenderer {
class C_VkBuffer : public Renderer::I_Buffer {
public:
	C_VkBuffer(const Renderer::BufferDescriptor& desc);

	void bind() override;

	friend class C_VkDevice;

private:
	VkBuffer m_Buffer;
};
} // namespace GLEngine::VkRenderer