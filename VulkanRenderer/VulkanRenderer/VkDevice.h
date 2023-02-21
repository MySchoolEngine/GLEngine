#pragma once

#include <Renderer/IDevice.h>

namespace GLEngine::VkRenderer {
class C_VkDevice : public Renderer::I_Device {
public:
	explicit C_VkDevice();
	~C_VkDevice();

	void Init(VkDevice_T* device);

	bool CreateView(VkImageView& result, VkImage& image, VkFormat format);

	VkDevice_T* GetVkDevice() { return m_Device; }

	bool			AllocateTexture(Renderer::I_DeviceTexture& texture) override;
	void			DestroyTexture(Renderer::I_DeviceTexture& texture) override;
	T_TextureHandle CreateTextureHandle(const Renderer::TextureDescriptor& desc) override;
	bool			AllocateSampler(Renderer::I_TextureSampler2D& texture) override;
	void			DestroySampler(Renderer::I_TextureSampler2D& texture) override;
	std::size_t		GetAllocatedMemory() const override;

private:
	VkDevice_T* m_Device;
};
} // namespace GLEngine::VkRenderer