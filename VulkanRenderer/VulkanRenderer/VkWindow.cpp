#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/Pipeline.h>
#include <VulkanRenderer/Shaders/ShaderCompiler.h>
#include <VulkanRenderer/VkDevice.h>
#include <VulkanRenderer/VkRenderer.h>
#include <VulkanRenderer/VkResourceManager.h>
#include <VulkanRenderer/VkTypeHelpers.h>
#include <VulkanRenderer/VkWindow.h>
#include <VulkanRenderer/VkWindowInfo.h>

#include <Renderer/Cameras/OrbitalCamera.h>
#include <Renderer/Components/StaticMeshHandles.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>
#include <Renderer/Viewport.h>
#include <Renderer/Windows/RayTrace.h>

#include <Entity/EntityManager.h>

#include <Core/EventSystem/Event/AppEvent.h>
#include <Core/EventSystem/EventDispatcher.h>
#include <Core/Resources/ResourceManager.h>

#include <Editor/Editors/ImageEditor.h>
#include <Editor/EntityEditor/EntitiesWindow.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

namespace GLEngine::VkRenderer {

static std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}
	size_t			  fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

//=================================================================================
C_VkWindow::C_VkWindow(const Core::S_WindowInfo& wndInfo)
	: GLFWManager::C_GLFWWindow(Core::E_Driver::Vulkan)
	, m_renderer(nullptr)
	, m_Instance(nullptr)
	, m_ImGUI(m_ID)
{
	Init(wndInfo);


	auto& rm = Core::C_ResourceManager::Instance();
	rm.RegisterResourceType(new Renderer::TextureLoader());
	rm.RegisterResourceType(new Renderer::MeshLoader());

	CreateWindowSurface();
	m_renderer = std::make_unique<C_VkRenderer>(m_Instance, m_Surface);

	CORE_LOG(E_Level::Info, E_Context::Render, "GLFW: Vulkan window initialized");
	CreateSwapChain();
	CreateImageViews();
	CreatePipeline();
	CreateFramebuffers();
	CreateCommandPool();
	CreateCommandBuffer();
	CreateSyncObjects();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateTexture();
	CreateTextureSampler();
	CreateDescriptorSets();

	m_ImGUI.OnAttach(); // manual call for now.

	// WORLD RELATED STUFF
	m_World = std::make_shared<Entity::C_EntityManager>();

	auto staticMeshHandle = m_World->GetOrCreateEntity("handles");
	{
		handlesMesh = std::make_shared<Renderer::C_StaticMeshHandles>();
		handlesMesh->SetParent(staticMeshHandle);
		staticMeshHandle->AddComponent(handlesMesh);
	}


	const SwapChainSupportDetails swapChainSupport = GetVkDevice().QuerySwapChainSupport(m_Surface);

	auto* pipeline = m_renderer->GetRMVK().GetPipeline(m_PipelineHandle);
	m_ImGUI.Init(VkGuiInit{
		.Instance		= m_Instance,
		.PhysicalDevice = GetVkDevice().GetPhysicalDevice(),
		.Device			= GetVkDevice().GetVkDevice(),
		.QueueFamily	= m_renderer->GetGraphicsFamilyIndex(),
		.Queue			= m_renderer->GetGraphicsQueue(),
		.PipelineCache	= VK_NULL_HANDLE,
		.MinImageCount	= swapChainSupport.capabilities.minImageCount,
		.ImageCount		= imageCount,
		.RenderPass		= pipeline->GetRenderPass(),
		.CommandBuffer	= m_CommandBuffer[0],
	});
}

//=================================================================================
C_VkWindow::~C_VkWindow()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		GetRenderer().GetRM().destroyBuffer(m_UniformBuffers[i]);
	}
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(m_renderer->GetDeviceVK(), m_ImageAvailableSemaphore[i], nullptr);
		vkDestroySemaphore(m_renderer->GetDeviceVK(), m_RenderFinishedSemaphore[i], nullptr);
		vkDestroyFence(m_renderer->GetDeviceVK(), m_InFlightFence[i], nullptr);
	}
	vkDestroyCommandPool(m_renderer->GetDeviceVK(), m_CommandPool, nullptr);
	for (auto framebuffer : m_SwapChainFramebuffers)
	{
		vkDestroyFramebuffer(m_renderer->GetDeviceVK(), framebuffer, nullptr);
	}
	DestroySwapchain();
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	vkDestroyDescriptorPool(m_renderer->GetDeviceVK(), descriptorPool, nullptr);
	// m_Pipeline.destroy(m_renderer->GetDevice());
	m_renderer.reset(nullptr);
	// image cleanup
	const auto* texture = m_renderer->GetRMVK().GetTexture(m_GPUTextureHandle);
	GetRenderer().GetRM().destroySampler(texture->GetSampler());
	GetRenderer().GetRM().destoryTexture(m_GPUTextureHandle);
};

//=================================================================================
void C_VkWindow::Update()
{
	m_ImGUI.FrameBegin();
	m_ImGUI.OnUpdate();
	bool show = true;
	ImGui::ShowDemoWindow(&show);
	Core::C_ResourceManager::Instance().UpdatePendingLoads();
	handlesMesh->Update();
	handlesMesh->Render(m_3DRenderer);

	vkWaitForFences(m_renderer->GetDeviceVK(), 1, &m_InFlightFence[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	auto	 ret = vkAcquireNextImageKHR(m_renderer->GetDeviceVK(), m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);

	UpdateUniformBuffer(currentFrame);

	vkResetFences(m_renderer->GetDeviceVK(), 1, &m_InFlightFence[currentFrame]);

	vkResetCommandBuffer(m_CommandBuffer[currentFrame], 0);
	RecordCommandBuffer(m_CommandBuffer[currentFrame], imageIndex);

	VkSemaphore			 waitSemaphores[]	= {m_ImageAvailableSemaphore[currentFrame]};
	VkSemaphore			 signalSemaphores[] = {m_RenderFinishedSemaphore[currentFrame]};
	VkPipelineStageFlags waitStages[]		= {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	const VkSubmitInfo submitInfo{
		.sType				  = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount	  = 1,
		.pWaitSemaphores	  = waitSemaphores,
		.pWaitDstStageMask	  = waitStages,
		.commandBufferCount	  = 1,
		.pCommandBuffers	  = &m_CommandBuffer[currentFrame],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores	  = signalSemaphores,
	};

	if (const auto result = vkQueueSubmit(m_renderer->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFence[currentFrame]) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to submit draw command buffer. {}", result);
		return;
	}

	VkSwapchainKHR		   swapChains[] = {m_SwapChain};
	const VkPresentInfoKHR presentInfo{
		.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores	= signalSemaphores,
		.swapchainCount		= 1,
		.pSwapchains		= swapChains,
		.pImageIndices		= &imageIndex,
	};

	vkQueuePresentKHR(m_renderer->GetPresentationQueue(), &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	m_3DRenderer.Clear();
}

//=================================================================================
void C_VkWindow::DestroySwapchain()
{
	for (auto imageView : m_SwapChainImagesViews)
	{
		vkDestroyImageView(m_renderer->GetDeviceVK(), imageView, nullptr);
	}
	vkDestroySwapchainKHR(m_renderer->GetDeviceVK(), m_SwapChain, nullptr);
}

//=================================================================================
void C_VkWindow::Init(const Core::S_WindowInfo& wndInfo)
{
	GLE_ASSERT(wndInfo.GetDriver() == Core::E_Driver::Vulkan, "This class supports only OpenGL");

	const auto vkInfo = dynamic_cast<const S_VkWindowInfo*>(&wndInfo);
	SetLayerDebugName(wndInfo.m_name);
	m_Instance = vkInfo->m_Instance;

	WindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	C_GLFWWindow::Init(wndInfo);
}

//=================================================================================
Renderer::I_Renderer& C_VkWindow::GetRenderer()
{
	return *(m_renderer.get());
}

//=================================================================================
bool C_VkWindow::CreateWindowSurface()
{
	const auto ret = glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);
	if (ret != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Window unable to initialise its surface. {}", ret);
		return false;
	}
	return true;
}

//=================================================================================
void C_VkWindow::CreateSwapChain()
{
	const SwapChainSupportDetails swapChainSupport = GetVkDevice().QuerySwapChainSupport(m_Surface);

	const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	const VkPresentModeKHR	 presentMode   = ChooseSwapPresentMode(swapChainSupport.presentModes);
	glm::ivec2				 size		   = GetSize();
	const VkExtent2D		 extent		   = ChooseSwapExtent(swapChainSupport.capabilities, {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)});

	imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface;

	createInfo.minImageCount	= imageCount;
	createInfo.imageFormat		= surfaceFormat.format;
	createInfo.imageColorSpace	= surfaceFormat.colorSpace;
	createInfo.imageExtent		= extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	m_renderer->FillFamilyIndexes(createInfo);

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	// blending with other windows
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	// clipping the pixels obstructed by other windows
	createInfo.presentMode = presentMode;
	createInfo.clipped	   = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (const auto result = vkCreateSwapchainKHR(m_renderer->GetDeviceVK(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create swap chain. {}", result);
		return;
	}


	vkGetSwapchainImagesKHR(m_renderer->GetDeviceVK(), m_SwapChain, &imageCount, nullptr);
	m_SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_renderer->GetDeviceVK(), m_SwapChain, &imageCount, m_SwapChainImages.data());

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent	   = extent;
}

//=================================================================================
VkSurfaceFormatKHR C_VkWindow::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	const auto it = std::find_if(availableFormats.begin(), availableFormats.end(), [](const auto& availableFormat) {
		return availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	});
	if (it != availableFormats.end())
		return *it;

	return availableFormats[0];
}

//=================================================================================
VkPresentModeKHR C_VkWindow::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	const auto it = std::find_if(availablePresentModes.begin(), availablePresentModes.end(),
								 [](const auto& availablePresentMode) { return availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR; });
	if (it != availablePresentModes.end())
		return *it;

	return VK_PRESENT_MODE_FIFO_KHR;
}

//=================================================================================
VkExtent2D C_VkWindow::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D actualExtent)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		actualExtent.width	= std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

//=================================================================================
void C_VkWindow::CreateImageViews()
{
	m_SwapChainImagesViews.resize(m_SwapChainImages.size());

	std::size_t i = 0;
	for (auto& image : m_SwapChainImages)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType						   = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image						   = image;
		createInfo.viewType						   = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format						   = m_SwapChainImageFormat;
		createInfo.components.r					   = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g					   = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b					   = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a					   = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel   = 0;
		createInfo.subresourceRange.levelCount	   = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount	   = 1;

		if (const auto result = vkCreateImageView(m_renderer->GetDeviceVK(), &createInfo, nullptr, &m_SwapChainImagesViews[i]) != VK_SUCCESS)
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create image view. {}", result);
			return;
		}
		++i;
	}
}

//=================================================================================
void C_VkWindow::OnEvent(Core::I_Event& event)
{
	// base can make filtering
	GLFWManager::C_GLFWWindow::OnEvent(event);

	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_AppEvent>(std::bind(&C_VkWindow::OnAppEvent, this, std::placeholders::_1));
	d.Dispatch<Core::C_WindowResizedEvent>(std::bind(&C_VkWindow::OnWindowResized, this, std::placeholders::_1));

	m_ImGUI.OnEvent(event);
}

//=================================================================================
bool C_VkWindow::OnWindowResized(Core::C_WindowResizedEvent& event)
{
	vkDestroyCommandPool(m_renderer->GetDeviceVK(), m_CommandPool, nullptr);
	for (auto framebuffer : m_SwapChainFramebuffers)
	{
		vkDestroyFramebuffer(m_renderer->GetDeviceVK(), framebuffer, nullptr);
	}
	DestroySwapchain();
	CreateSwapChain();
	CreatePipeline();
	CreateFramebuffers();
	CreateCommandPool();
	CreateCommandBuffer();
	CreateSyncObjects();


	return true;
}

//=================================================================================
void C_VkWindow::CreatePipeline()
{
	Renderer::PipelineDescriptor desc{
		.primitiveType = Renderer::E_RenderPrimitives::TriangleList,
		.bindingCount = 3,
		.vertexInput   = {{
							  .binding = 0,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec4>
						  },
						  {
							  .binding = 1,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec3>
						  },
						  {
							  .binding = 2,
							  .type	   = Renderer::T_TypeShaderDataType_v<glm::vec2>
						  },},
		.shader = "basicVulkan",
		.colorAttachementFormat = GetTextureFormat(m_SwapChainImageFormat),
	};
	m_PipelineHandle = m_renderer->GetRM().createPipeline(desc);
}

//=================================================================================
void C_VkWindow::CreateFramebuffers()
{
	auto* pipeline = m_renderer->GetRMVK().GetPipeline(m_PipelineHandle);
	m_SwapChainFramebuffers.resize(m_SwapChainImagesViews.size());
	for (size_t i = 0; i < m_SwapChainImagesViews.size(); i++)
	{
		VkImageView attachments[] = {m_SwapChainImagesViews[i]};

		const VkFramebufferCreateInfo framebufferInfo{
			.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass		 = pipeline->GetRenderPass(),
			.attachmentCount = 1,
			.pAttachments	 = attachments,
			.width			 = m_SwapChainExtent.width,
			.height			 = m_SwapChainExtent.height,
			.layers			 = 1,
		};

		if (const auto result = vkCreateFramebuffer(m_renderer->GetDeviceVK(), &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create render pass. {}", result);
			return;
		}
	}
}

//=================================================================================
void C_VkWindow::CreateCommandPool()
{
	const VkCommandPoolCreateInfo poolInfo{
		.sType			  = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags			  = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = m_renderer->GetGraphicsFamilyIndex(),
	};

	if (const auto result = vkCreateCommandPool(m_renderer->GetDeviceVK(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create command pool. {}", result);
		return;
	}
}

//=================================================================================
void C_VkWindow::CreateCommandBuffer()
{
	m_CommandBuffer.resize(MAX_FRAMES_IN_FLIGHT);

	const VkCommandBufferAllocateInfo allocInfo{
		.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool		= m_CommandPool,
		.level				= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = static_cast<uint32_t>(m_CommandBuffer.size()),
	};

	if (const auto result = vkAllocateCommandBuffers(m_renderer->GetDeviceVK(), &allocInfo, m_CommandBuffer.data()) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create command buffer. {}", result);
	}
}

//=================================================================================
void C_VkWindow::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	const VkCommandBufferBeginInfo beginInfo{
		.sType			  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags			  = 0,		 // Optional
		.pInheritanceInfo = nullptr, // Optional
	};

	if (const auto result = vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to begin recording command buffer. {}", result);
		return;
	}

	auto*						pipeline   = m_renderer->GetRMVK().GetPipeline(m_PipelineHandle);
	const VkClearValue			clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	const VkRenderPassBeginInfo renderPassInfo{
		.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass		 = pipeline->GetRenderPass(),
		.framebuffer	 = m_SwapChainFramebuffers[imageIndex],
		.renderArea		 = {{0, 0}, m_SwapChainExtent},
		.clearValueCount = 1,
		.pClearValues	 = &clearColor,
	};

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetGraphicsPipeline());

	// we went for dynamic state of viewport and scissor
	const Renderer::C_Viewport viewport(0, 0, GetSize());
	const VkViewport		   vkViewport = TranslateViewport(viewport);
	vkCmdSetViewport(commandBuffer, 0, 1, &vkViewport);
	const VkRect2D scissor{.offset = {0, 0}, .extent = {viewport.GetResolution().x, viewport.GetResolution().y}};
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	m_RenderInterface.BeginRender(commandBuffer, descriptorSets[currentFrame], pipeline);
	m_3DRenderer.Commit(m_RenderInterface);
	m_RenderInterface.EndRender();

	m_ImGUI.SetCommandBuffer(m_CommandBuffer[currentFrame]);
	m_ImGUI.FrameEnd(m_Input);

	vkCmdEndRenderPass(commandBuffer);
	if (const auto result = vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to record command buffer. {}", result);
		return;
	}
}

//=================================================================================
void C_VkWindow::CreateSyncObjects()
{
	m_ImageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
	m_RenderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
	m_InFlightFence.resize(MAX_FRAMES_IN_FLIGHT);

	const VkSemaphoreCreateInfo semaphoreInfo{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	};
	const VkFenceCreateInfo fenceInfo{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(m_renderer->GetDeviceVK(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore[i]) != VK_SUCCESS
			|| vkCreateSemaphore(m_renderer->GetDeviceVK(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore[i]) != VK_SUCCESS
			|| vkCreateFence(m_renderer->GetDeviceVK(), &fenceInfo, nullptr, &m_InFlightFence[i]) != VK_SUCCESS)
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "failed to record command buffer. {}");
		}
	}
}

//=================================================================================
void C_VkWindow::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	m_UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_UniformBuffers[i] = GetRenderer().GetRM().createBuffer(Renderer::BufferDescriptor{
			.size  = static_cast<uint32_t>(bufferSize),
			.type  = Renderer::E_BufferType::Uniform,
			.usage = Renderer::E_ResourceUsage::Persistent,
		});
	}
}

//=================================================================================
void C_VkWindow::UpdateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto				currentTime = std::chrono::high_resolution_clock::now();
	float				time		= std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj  = glm::perspective(glm::radians(45.0f), m_SwapChainExtent.width / (float)m_SwapChainExtent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	m_renderer->SetBufferData(m_UniformBuffers[currentImage], sizeof(ubo), &ubo);
}

//=================================================================================
void C_VkWindow::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type			 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	poolSizes[1].type			 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	const VkDescriptorPoolCreateInfo poolInfo{
		.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags		   = 0,
		.maxSets	   = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
		.poolSizeCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
		.pPoolSizes	   = poolSizes.data(),
	};
	if (const auto result = vkCreateDescriptorPool(m_renderer->GetDeviceVK(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to create descriptor pool. {}", result);
		return;
	}
}

//=================================================================================
void C_VkWindow::CreateDescriptorSets()
{
	auto*							   pipeline = m_renderer->GetRMVK().GetPipeline(m_PipelineHandle);
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, pipeline->GetDescriptorSetLayout());
	const VkDescriptorSetAllocateInfo  allocInfo{
		 .sType				 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		 .descriptorPool	 = descriptorPool,
		 .descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
		 .pSetLayouts		 = layouts.data(),
	 };

	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (const auto result = vkAllocateDescriptorSets(m_renderer->GetDeviceVK(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to allocate descriptor sets. {}", result);
		return;
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		C_VkBuffer*					 uniformBuffer = m_renderer->GetRMVK().GetBuffer(m_UniformBuffers[i]);
		const VkDescriptorBufferInfo bufferInfo{
			.buffer = uniformBuffer->GetBuffer(),
			.offset = 0,
			.range	= sizeof(UniformBufferObject),
		};
		const auto*					texture = m_renderer->GetRMVK().GetTexture(m_GPUTextureHandle);
		const auto*					sampler = m_renderer->GetRMVK().GetSampler(texture->GetSampler());
		const VkDescriptorImageInfo imageInfo{
			.sampler	 = sampler->GetVkSampler(),
			.imageView	 = texture->GetView(),
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		};

		const VkWriteDescriptorSet descriptorUniform{
			.sType			 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet			 = descriptorSets[i],
			.dstBinding		 = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType	 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo	 = &bufferInfo,
		};

		const VkWriteDescriptorSet descriptorImage{
			.sType			 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet			 = descriptorSets[i],
			.dstBinding		 = 1,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType	 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo		 = &imageInfo,
		};

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{descriptorUniform, descriptorImage};

		vkUpdateDescriptorSets(m_renderer->GetDeviceVK(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

//=================================================================================
C_VkDevice& C_VkWindow::GetVkDevice()
{
	return dynamic_cast<C_VkDevice&>(m_renderer->GetDevice());
}

//=================================================================================
void C_VkWindow::CreateTexture()
{
	auto& rm = Core::C_ResourceManager::Instance();

	m_TextureHandle		= rm.LoadResource<Renderer::TextureResource>(std::filesystem::path("Models/Error.bmp"), true);
	const auto& storage = m_TextureHandle.GetResource().GetStorage();

	const glm::uvec2   dim		 = storage.GetDimensions();
	const VkDeviceSize imageSize = dim.x * dim.y * 4 * sizeof(float);

	Renderer::TextureDescriptor textureDesc{.name		   = m_TextureHandle.GetResource().GetFilePath().generic_string(),
											.width		   = storage.GetDimensions().x,
											.height		   = storage.GetDimensions().y,
											.type		   = Renderer::E_TextureType::TEXTURE_2D,
											.format		   = Renderer::GetClosestFormat(storage.GetChannels(), !Renderer::IsIntegral(storage.GetStorageType())),
											.m_bStreamable = false};

	m_GPUTextureHandle = GetRenderer().GetRM().createTexture(textureDesc);
	m_renderer->SetTextureData(m_GPUTextureHandle, storage);
}

//=================================================================================
void C_VkWindow::CreateTextureSampler()
{
	auto GPUSamplerHandle = m_renderer->GetRM().createSampler(Renderer::SamplerDescriptor2D{
		.m_FilterMin = Renderer::E_TextureFilter::Linear,
		.m_FilterMag = Renderer::E_TextureFilter::Linear,
		.m_WrapS	 = Renderer ::E_WrapFunction::Repeat,
		.m_WrapT	 = Renderer ::E_WrapFunction::Repeat,
		.m_WrapU	 = Renderer ::E_WrapFunction::Repeat,
	});
	if (auto* texture = m_renderer->GetRMVK().GetTexture(m_GPUTextureHandle))
	{
		texture->SetSampler(GPUSamplerHandle);
	}
}

//=================================================================================
void C_VkWindow::SetupGUI()
{
	auto player = m_World->GetOrCreateEntity("Player");
	if (player)
	{
		auto  cameras = player->GetComponents(Entity::E_ComponentType::Camera);
		float zoom	  = 5.0f;
		playerCamera  = std::make_shared<Renderer::Cameras::C_OrbitalCamera>(player);
		playerCamera->SetupCameraProjection(0.1f, 2 * zoom * 100, static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()), 90.0f);
		playerCamera->SetupCameraView(zoom, glm::vec3(0.0f), 90, 0);
		playerCamera->Update();
		player->AddComponent(playerCamera);
	}

	auto& guiMGR = m_ImGUI.GetGUIMgr();


	// Entity window
	{
		m_EntitiesWindowGUID = NextGUID();

		auto entitiesWindow = new Editor::C_EntitiesWindow(m_EntitiesWindowGUID, m_World);
		guiMGR.AddCustomWindow(entitiesWindow);
		entitiesWindow->SetVisible();
	}

	// RT window
	{
		m_RayTraceGUID = NextGUID();

		m_RayTraceWindow = new Renderer::C_RayTraceWindow(m_RayTraceGUID, playerCamera, guiMGR);

		guiMGR.AddCustomWindow(m_RayTraceWindow);
		m_RayTraceWindow->SetVisible();
	}

	// Image editor
	{
		m_ImageEditorGUID = NextGUID();

		auto* imageEditorWindow = new Editor::C_ImageEditor(m_ImageEditorGUID, guiMGR);

		guiMGR.AddCustomWindow(imageEditorWindow);
		imageEditorWindow->SetVisible();
	}
}

//=================================================================================
bool C_VkWindow::OnAppEvent(Core::C_AppEvent& event)
{
	if (event.GetEventType() == Core::C_AppEvent::E_Type::AppInit)
	{
		SetupGUI();
		return true;
	}
	return false;
}

} // namespace GLEngine::VkRenderer
