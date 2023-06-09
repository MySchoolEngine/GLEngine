#pragma once

#include <VulkanRenderer/VkResourceManager.h>

#include <Renderer/IDevice.h>

namespace GLEngine::VkRenderer {

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR		capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>	presentModes;
};

class C_VkDevice : public Renderer::I_Device {
public:
	explicit C_VkDevice();
	~C_VkDevice();

	void Init(VkDevice_T* device, VkPhysicalDevice_T* gpu);

	bool CreateView(VkImageView& result, Renderer::Handle<Renderer::Texture> texture);

	VkDevice_T* GetVkDevice() { return m_Device; }

	bool			AllocateTexture(Renderer::I_DeviceTexture& texture) override;
	void			DestroyTexture(Renderer::I_DeviceTexture& texture) override;
	T_TextureHandle CreateTextureHandle(const Renderer::TextureDescriptor& desc) override;
	bool			AllocateSampler(Renderer::I_TextureSampler2D& texture) override;
	void			DestroySampler(Renderer::I_TextureSampler2D& texture) override;
	std::size_t		GetAllocatedMemory() const override;
	void			CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void			CreateImage(glm::uvec2 dim, VkBufferUsageFlags usage, VkImage& textureImage, VkDeviceMemory& textureImageMemory);

	SwapChainSupportDetails QuerySwapChainSupport(VkSurfaceKHR surface);
	uint32_t				findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	C_VkResourceManager& GetRM();

private:
	VkDevice_T*			m_Device = nullptr;
	VkPhysicalDevice_T* m_GPU	 = nullptr;

	C_VkResourceManager m_GPUResourceManager;
};
} // namespace GLEngine::VkRenderer