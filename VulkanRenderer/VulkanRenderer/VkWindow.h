#pragma once

#include <VulkanRenderer/Pipeline.h>

#include <Core/Application.h>
#include <Core/IWindow.h>

#include <GLFWWindowManager/GLFWWindow.h>

struct GLFWwindow;

namespace GLEngine::Core {
;
class C_WindowResizedEvent;
} // namespace GLEngine::Core

namespace GLEngine::VkRenderer {

class C_VkRenderer;

class C_VkWindow : public GLFWManager::C_GLFWWindow {
public:
	C_VkWindow(const Core::S_WindowInfo& wndInfo);
	virtual ~C_VkWindow();

	virtual Renderer::I_Renderer& GetRenderer() override;

	virtual void OnEvent(Core::I_Event& event) override;
	void		 Update() override;

protected:
	virtual void				  Init(const Core::S_WindowInfo& wndInfo) override;
	std::unique_ptr<C_VkRenderer> m_renderer;

	bool OnWindowResized(Core::C_WindowResizedEvent& event);

private:
	bool			   CreateWindowSurface();
	void			   CreateSwapChain();
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR   ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D		   ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D actualExtent);
	void			   CreateImageViews();
	void			   CreatePipeline();
	void			   CreateFramebuffers();
	void			   CreateCommandPool();
	void			   CreateCommandBuffer();
	void			   CreateSyncObjects();
	void			   CreateVertexBuffer();
	void			   RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void			   CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void			   CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void DestroySwapchain();

	VkSwapchainKHR			   m_SwapChain;
	std::vector<VkImage>	   m_SwapChainImages;
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;
	std::vector<VkImageView>   m_SwapChainImagesViews;
	VkFormat				   m_SwapChainImageFormat;
	VkExtent2D				   m_SwapChainExtent;
	VkCommandPool			   m_CommandPool;
	VkCommandBuffer			   m_CommandBuffer; // auto cleanup on pool release!

	C_Pipeline m_Pipeline;

	VkSemaphore	   m_ImageAvailableSemaphore;
	VkSemaphore	   m_RenderFinishedSemaphore;
	VkFence		   m_InFlightFence;
	VkBuffer	   m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMemory;

	VkSurfaceKHR_T* m_Surface;

	VkInstance_T* m_Instance;
};
} // namespace GLEngine::VkRenderer
