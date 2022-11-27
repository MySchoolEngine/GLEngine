#pragma once

#include <Renderer/IDevice.h>

namespace GLEngine::VkRenderer {
class C_VkDevice : public Renderer::I_Device {
public:
	C_VkDevice(VkDevice_T* device);
	~C_VkDevice();

	bool			AllocateTexture(Renderer::I_DeviceTexture& texture) override;
	void			DestroyTexture(Renderer::I_DeviceTexture& texture) override;
	T_TextureHandle CreateTextureHandle(const Renderer::TextureDescriptor& desc) override;

	bool AllocateSampler(Renderer::I_TextureSampler2D& texture) override;
	void DestroySampler(Renderer::I_TextureSampler2D& texture) override;

	std::size_t GetAllocatedMemory() const override { return 0; }

	bool		   AllocateBuffer(Renderer::I_Buffer& buffer) override;
	T_BufferHandle CreateBufferHandle(const Renderer::BufferDescriptor& desc) override;
	void		   DestroyBuffer(Renderer::I_Buffer& buffer) override;

	VkDevice_T* GetDeviceVK() { return m_Device; }

private:
	VkDevice_T* m_Device;
};
} // namespace GLEngine::VkRenderer
