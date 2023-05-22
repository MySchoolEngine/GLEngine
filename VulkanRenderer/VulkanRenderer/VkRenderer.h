#pragma once

#include <VulkanRenderer/VkDevice.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/TextureResource.h>

#include <Core/CoreMacros.h>
#include <Core/Resources/ResourceHandle.h>

namespace GLEngine::VkRenderer {

// VkRenderer is tightly bound to its window now
class C_VkRenderer : public Renderer::I_Renderer {
public:
	// TODO!! SRUFACE SHOULD NOT GO THERE!
	C_VkRenderer(VkInstance instance, VkSurfaceKHR surface);
	virtual ~C_VkRenderer() override;

	//=================================================
	// Renderer::I_Renderer
	//=================================================
	virtual void Lock(bool lock = true) override;
	virtual void AddCommand(T_CommandPtr) override;
	virtual void AddBatch(T_BatchPtr) override;
	virtual void SortCommands() override;
	virtual void ExtractData() override;
	virtual void TransformData() override;
	virtual void Commit() const override;
	virtual void ClearCommandBuffers() override;
	virtual void AddTransferCommand(T_CommandPtr) override;


	//=================================================================================
	virtual Renderer::E_PassType GetCurrentPassType() const override;

	VkDevice_T*					GetDeviceVK() { return m_VkDevice; }
	virtual Renderer::I_Device& GetDevice() override;

	void FillFamilyIndexes(VkSwapchainCreateInfoKHR& createInfo);

	// todo: Temporary
	uint32_t GetGraphicsFamilyIndex() const;
	VkQueue	 GetGraphicsQueue() const;
	VkQueue	 GetTransferQueue() const;
	VkQueue	 GetPresentationQueue() const;

	void CopyBuffer(VkBuffer srcBuffer, Renderer::Handle<Renderer::Buffer> dstBuffer, VkDeviceSize size, VkCommandPool& commandPool);
	// copy whole image, blocking operation, should go to async, have options to copy just parts of image etc.
	void CopyImageResource(Renderer::Handle<Renderer::Texture> dstTexture, Core::ResourceHandle<Renderer::TextureResource>& resource, VkCommandPool& commandPool);
	void CopyBufferToImage(VkBuffer srcBuffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool& commandPool);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandPool& commandPool);

private:
	bool											 m_Locked = false;
	std::vector<Renderer::I_Renderer::T_CommandPtr>* m_CommandQueue;

	VkInstance_T* m_Instance;
	C_VkDevice	  m_Device;
	VkDevice_T*	  m_VkDevice;
	uint32_t	  m_GraphicsFamilyIndex;
	uint32_t	  m_ComputeFamilyIndex;
	uint32_t	  m_TransferFamilyIndex = (uint32_t)-1;
	uint32_t	  m_PresentingFamilyIndex;

	VkQueue m_graphicsQueue;
	VkQueue m_TransferQueue;
	VkQueue m_presentQueue;

	bool								 InitDevice(VkSurfaceKHR surface);
	std::vector<VkDeviceQueueCreateInfo> CreatePresentingQueueInfos();

	VkCommandBuffer BeginSingleTimeCommands(VkCommandPool& commandPool);
	void			EndSigneTimeCommands(VkCommandBuffer& commandBuffer, VkCommandPool& commandPool);
};

} // namespace GLEngine::VkRenderer