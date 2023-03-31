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
struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

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
	void			   CreateIndexBuffer();
	void			   CreateUniformBuffers();
	void			   CreateDescriptorPool();
	void			   CreateDescriptorSets();
	void			   RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	void DestroySwapchain();

	void UpdateUniformBuffer(uint32_t currentImage);

	VkSwapchainKHR				 m_SwapChain;
	std::vector<VkImage>		 m_SwapChainImages;
	std::vector<VkFramebuffer>	 m_SwapChainFramebuffers;
	std::vector<VkImageView>	 m_SwapChainImagesViews;
	VkFormat					 m_SwapChainImageFormat;
	VkExtent2D					 m_SwapChainExtent;
	VkCommandPool				 m_CommandPool;
	std::vector<VkCommandBuffer> m_CommandBuffer; // auto cleanup on pool release!

	C_Pipeline m_Pipeline;

	std::vector<VkSemaphore> m_ImageAvailableSemaphore;
	std::vector<VkSemaphore> m_RenderFinishedSemaphore;
	std::vector<VkFence>	 m_InFlightFence;
	VkBuffer				 m_VertexBuffer;
	VkDeviceMemory			 m_VertexBufferMemory;
	VkBuffer				 m_IndexBuffer;
	VkDeviceMemory			 m_IndexBufferMemory;

	const int MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t  currentFrame		   = 0;


	// Uniform buffers
	std::vector<VkBuffer>		 uniformBuffers;
	std::vector<VkDeviceMemory>	 uniformBuffersMemory;
	std::vector<void*>			 uniformBuffersMapped;
	VkDescriptorPool			 descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	VkSurfaceKHR_T* m_Surface;

	VkInstance_T* m_Instance;
};
} // namespace GLEngine::VkRenderer
