#pragma once

#include <VulkanRenderer/VkDevice.h>

#include <Renderer/IRenderer.h>

#include <Core/CoreMacros.h>

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

	void					FillFamilyIndexes(VkSwapchainCreateInfoKHR& createInfo);

	// todo: Temporary
	uint32_t GetGraphicsFamilyIndex() const;
	VkQueue	 GetGraphicsQueue() const;
	VkQueue	 GetPresentationQueue() const;

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool& commandPool);

private:
	bool											 m_Locked = false;
	std::vector<Renderer::I_Renderer::T_CommandPtr>* m_CommandQueue;

	VkInstance_T*		m_Instance;
	C_VkDevice			m_Device;
	VkDevice_T*			m_VkDevice;
	uint32_t			m_GraphicsFamilyIndex;
	uint32_t			m_ComputeFamilyIndex;
	uint32_t			m_PresentingFamilyIndex;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	bool								 InitDevice(VkSurfaceKHR surface);
	std::vector<VkDeviceQueueCreateInfo> CreatePresentingQueueInfos();

	VkCommandBuffer BeginSingleTimeCommands(VkCommandPool& commandPool);
	void			EndSigneTimeCommands(VkCommandBuffer& commandBuffer, VkCommandPool& commandPool);
};

} // namespace GLEngine::VkRenderer