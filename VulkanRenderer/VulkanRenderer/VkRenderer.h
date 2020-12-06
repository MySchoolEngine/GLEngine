#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderer.h>

namespace GLEngine::VkRenderer {

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR		capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>	presentModes;
};

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


	//=================================================================================
	virtual Renderer::E_PassType GetCurrentPassType() const override;

	VkDevice_T* GetDevice() { return m_Device; }

	SwapChainSupportDetails QuerySwapChainSupport(VkSurfaceKHR surface);
	void FillFamilyIndexes(VkSwapchainCreateInfoKHR& createInfo);
private:

	bool m_Locked = false;
	std::vector<Renderer::I_Renderer::T_CommandPtr>* m_CommandQueue;

	VkInstance_T*		m_Instance;
	VkDevice_T*			m_Device;
	VkPhysicalDevice_T* m_GPU;
	uint32_t			m_GraphicsFamilyIndex;
	uint32_t			m_ComputeFamilyIndex;
	uint32_t			m_PresentingFamilyIndex;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	bool InitDevice(VkSurfaceKHR surface);
	std::vector<VkDeviceQueueCreateInfo> CreatePresentingQueueInfos();

};

}