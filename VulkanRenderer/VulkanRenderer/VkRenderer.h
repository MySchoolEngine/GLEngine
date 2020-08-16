#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderer.h>

namespace GLEngine::VkRenderer {

// VkRenderer is tightly bound to its window now
class C_VkRenderer : public Renderer::I_Renderer {
public:
	C_VkRenderer(VkInstance instance, GLFWwindow* window);
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

private:
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR		capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR>	presentModes;
	};

	bool m_Locked = false;
	std::vector<Renderer::I_Renderer::T_CommandPtr>* m_CommandQueue;

	VkInstance_T*		m_Instance;
	VkDevice_T*			m_Device;
	VkPhysicalDevice_T* m_GPU;
	uint32_t			m_GraphicsFamilyIndex;
	uint32_t			m_ComputeFamilyIndex;
	uint32_t			m_PresentingFamilyIndex;
	GLFWwindow*			m_Window;
	VkSurfaceKHR_T*		m_Surface;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	VkSwapchainKHR m_SwapChain;
	std::vector<VkImage> m_SwapChainImages;
	std::vector<VkImageView> m_SwapChainImagesViews;
	VkFormat	m_SwapChainImageFormat;
	VkExtent2D	m_SwapChainExtent;

	bool InitDevice();
	bool CreateWindowSurface();
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D actualExtent);
	void CreateSwapChain();
	void CreateImageViews();
	std::vector<VkDeviceQueueCreateInfo> CreatePresentingQueueInfos();

};

}