#pragma once

#include <VulkanRenderer/ImGui/VkImGUILayer.h>
#include <VulkanRenderer/VkRenderInterface.h>
#include <VulkanRenderer/VkResourceManager.h>

#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Renderer3D.h>
#include <Renderer/Textures/TextureResource.h>

#include <Core/Application.h>
#include <Core/IWindow.h>
#include <Core/Resources/ResourceHandle.h>

#include <GLFWWindowManager/GLFWWindow.h>

struct GLFWwindow;

namespace GLEngine::Core {
;
class C_WindowResizedEvent;
} // namespace GLEngine::Core

namespace GLEngine::Renderer {
class I_CameraComponent;
class C_StaticMeshHandles;
class C_RayTraceWindow;
namespace Cameras {
class C_OrbitalCamera;
}
} // namespace GLEngine::Renderer

namespace GLEngine::Entity {
class C_EntityManager;
}

namespace GLEngine::VkRenderer {

class C_VkRenderer;
class C_VkImGUILayer;
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
	bool			   OnAppEvent(Core::C_AppEvent& event);
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
	void			   CreateUniformBuffers();
	void			   CreateDescriptorPool();
	void			   CreateDescriptorSets();
	void			   CreateTexture();
	void			   CreateTextureSampler();
	void			   SetupGUI();
	void			   RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	void DestroySwapchain();

	void UpdateUniformBuffer(uint32_t currentImage);

	C_VkDevice& GetVkDevice();

	VkSwapchainKHR				 m_SwapChain;
	std::vector<VkImage>		 m_SwapChainImages;
	std::vector<VkFramebuffer>	 m_SwapChainFramebuffers;
	std::vector<VkImageView>	 m_SwapChainImagesViews;
	VkFormat					 m_SwapChainImageFormat;
	VkExtent2D					 m_SwapChainExtent;
	VkCommandPool				 m_CommandPool;
	std::vector<VkCommandBuffer> m_CommandBuffer; // auto cleanup on pool release!

	C_VkImGUILayer m_ImGUI;

	Renderer::Handle<Renderer::Pipeline> m_PipelineHandle;

	std::vector<VkSemaphore>		   m_ImageAvailableSemaphore;
	std::vector<VkSemaphore>		   m_RenderFinishedSemaphore;
	std::vector<VkFence>			   m_InFlightFence;

	Core::ResourceHandle<Renderer::MeshResource>	m_MeshHandle;
	Core::ResourceHandle<Renderer::TextureResource> m_TextureHandle;

	Renderer::Handle<Renderer::Texture> m_GPUTextureHandle;

	const int MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t  currentFrame		   = 0;


	// Uniform buffers
	std::vector<Renderer::Handle<Renderer::Buffer>> m_UniformBuffers;
	VkDescriptorPool								descriptorPool;
	std::vector<VkDescriptorSet>					descriptorSets;

	VkSurfaceKHR_T* m_Surface;

	VkInstance_T* m_Instance;

	uint32_t imageCount;

	GUID m_EntitiesWindowGUID;
	GUID						m_RayTraceGUID;
	GUID						m_ImageEditorGUID;
	Renderer::C_RayTraceWindow* m_RayTraceWindow;


	// SCENE RELATED STUFF
	Renderer::Renderer3D						   m_3DRenderer;
	std::shared_ptr<Entity::C_EntityManager>	   m_World;
	std::shared_ptr<Renderer::C_StaticMeshHandles> handlesMesh;
	std::shared_ptr<Renderer::Cameras::C_OrbitalCamera> playerCamera;

	C_VkRenderInterface m_RenderInterface;
};
} // namespace GLEngine::VkRenderer
