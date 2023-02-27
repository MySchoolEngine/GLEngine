#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/Shaders/ShaderCompiler.h>
#include <VulkanRenderer/VkRenderer.h>
#include <VulkanRenderer/VkTypeHelpers.h>
#include <VulkanRenderer/VkWindow.h>
#include <VulkanRenderer/VkWindowInfo.h>

#include <Renderer/Viewport.h>

#include <Core/EventSystem/Event/AppEvent.h>
#include <Core/EventSystem/EventDispatcher.h>

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
	: m_renderer(nullptr)
	, m_Instance(nullptr)
{
	Init(wndInfo);

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
}

//=================================================================================
C_VkWindow::~C_VkWindow()
{
	vkDestroySemaphore(m_renderer->GetDeviceVK(), m_ImageAvailableSemaphore, nullptr);
	vkDestroySemaphore(m_renderer->GetDeviceVK(), m_RenderFinishedSemaphore, nullptr);
	vkDestroyFence(m_renderer->GetDeviceVK(), m_InFlightFence, nullptr);
	vkDestroyCommandPool(m_renderer->GetDeviceVK(), m_CommandPool, nullptr);
	for (auto framebuffer : m_SwapChainFramebuffers)
	{
		vkDestroyFramebuffer(m_renderer->GetDeviceVK(), framebuffer, nullptr);
	}
	DestroySwapchain();
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	vkDestroyPipeline(m_renderer->GetDeviceVK(), m_GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_renderer->GetDeviceVK(), m_PipelineLayout, nullptr);
	vkDestroyRenderPass(m_renderer->GetDeviceVK(), m_RenderPass, nullptr);
	m_renderer.reset(nullptr);
};

//=================================================================================
void C_VkWindow::Update()
{
	vkWaitForFences(m_renderer->GetDeviceVK(), 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(m_renderer->GetDeviceVK(), 1, &m_InFlightFence);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(m_renderer->GetDeviceVK(), m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	vkResetCommandBuffer(m_CommandBuffer, 0);
	RecordCommandBuffer(m_CommandBuffer, imageIndex);

	VkSemaphore			 waitSemaphores[]	= {m_ImageAvailableSemaphore};
	VkSemaphore			 signalSemaphores[] = {m_RenderFinishedSemaphore};
	VkPipelineStageFlags waitStages[]		= {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	const VkSubmitInfo submitInfo{
		.sType				  = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount	  = 1,
		.pWaitSemaphores	  = waitSemaphores,
		.pWaitDstStageMask	  = waitStages,
		.commandBufferCount	  = 1,
		.pCommandBuffers	  = &m_CommandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores	  = signalSemaphores,
	};

	if (const auto result = vkQueueSubmit(m_renderer->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFence) != VK_SUCCESS)
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
	const SwapChainSupportDetails swapChainSupport = m_renderer->QuerySwapChainSupport(m_Surface);

	const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	const VkPresentModeKHR	 presentMode   = ChooseSwapPresentMode(swapChainSupport.presentModes);
	glm::ivec2				 size		   = GetSize();
	const VkExtent2D		 extent		   = ChooseSwapExtent(swapChainSupport.capabilities, {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)});

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
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
	for (const auto& image : m_SwapChainImages)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format	= m_SwapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel   = 0;
		createInfo.subresourceRange.levelCount	   = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount	   = 1;

		if (const auto result = vkCreateImageView(m_renderer->GetDeviceVK(), &createInfo, nullptr, &m_SwapChainImagesViews[i]) != VK_SUCCESS)
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create image views. {}", result);
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
	d.Dispatch<Core::C_WindowResizedEvent>(std::bind(&C_VkWindow::OnWindowResized, this, std::placeholders::_1));
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
	vkDestroyPipeline(m_renderer->GetDeviceVK(), m_GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_renderer->GetDeviceVK(), m_PipelineLayout, nullptr);
	vkDestroyRenderPass(m_renderer->GetDeviceVK(), m_RenderPass, nullptr);
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
	std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

	const VkPipelineDynamicStateCreateInfo dynamicState{
		.sType			   = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pNext			   = nullptr,
		.flags			   = 0,
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates	   = dynamicStates.data(),
	};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount	= 0;
	vertexInputInfo.pVertexBindingDescriptions		= nullptr; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions	= nullptr; // Optional

	const VkPipelineInputAssemblyStateCreateInfo inputAssembly{
		.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology				= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE,
	};

	const VkPipelineViewportStateCreateInfo viewportState{
		.sType		   = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.scissorCount  = 1,
	};

	const VkPipelineRasterizationStateCreateInfo rasterizer{
		.sType					 = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable		 = VK_FALSE, // for shadow-maps set true and enable GPU feature
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode			 = VK_POLYGON_MODE_FILL, // wireframe also needs GPU feature to change
		.cullMode				 = VK_CULL_MODE_BACK_BIT,
		.frontFace				 = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable		 = VK_FALSE,
		.depthBiasConstantFactor = 0.0f, // Optional
		.depthBiasClamp			 = 0.0f, // Optional
		.depthBiasSlopeFactor	 = 0.0f, // Optional
		.lineWidth				 = 1.0f,
	};

	const VkPipelineMultisampleStateCreateInfo multisampling{
		.sType				   = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable   = VK_FALSE,
		.minSampleShading	   = 1.0f,	   // Optional
		.pSampleMask		   = nullptr,  // Optional
		.alphaToCoverageEnable = VK_FALSE, // Optional
		.alphaToOneEnable	   = VK_FALSE, // Optional
	};

	const VkPipelineColorBlendAttachmentState colorBlendAttachment{
		.blendEnable		 = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,	 // Optional
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
		.colorBlendOp		 = VK_BLEND_OP_ADD,		 // Optional
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,	 // Optional
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
		.alphaBlendOp		 = VK_BLEND_OP_ADD,		 // Optional
		.colorWriteMask		 = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
	};

	const VkPipelineColorBlendStateCreateInfo colorBlending{
		.sType			 = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable	 = VK_FALSE,
		.logicOp		 = VK_LOGIC_OP_COPY, // Optional
		.attachmentCount = 1,
		.pAttachments	 = &colorBlendAttachment,
		.blendConstants	 = {0.0f, 0.0f, 0.0f, 0.0f}, // Optional
	};

	C_ShaderCompiler						 compiler(static_cast<C_VkDevice&>(m_renderer->GetDevice()));
	Renderer::C_ShaderLoader<VkShaderModule> loader(compiler);

	std::vector<std::pair<Renderer::E_ShaderStage, VkShaderModule>> stages;
	if (!loader.LoadAllStages("basicVulkan", stages))
	{
		// error probably? End the run?
	}
	if (!compiler.linkProgram(m_PipelineLayout, stages))
	{
		return;
	}
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	for (const auto& stage : stages)
	{
		const VkPipelineShaderStageCreateInfo shaderStageInfo{
			.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext	= nullptr,
			.flags	= 0,
			.stage	= GetVkShaderStage(stage.first),
			.module = stage.second,
			.pName	= "main", // entry point
		};
		shaderStages.push_back(shaderStageInfo);
	}

	CreateRenderPass();

	const VkGraphicsPipelineCreateInfo pipelineInfo{
		.sType				 = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount			 = 2,
		.pStages			 = shaderStages.data(),
		.pVertexInputState	 = &vertexInputInfo,
		.pInputAssemblyState = &inputAssembly,
		.pViewportState		 = &viewportState,
		.pRasterizationState = &rasterizer,
		.pMultisampleState	 = &multisampling,
		.pDepthStencilState	 = nullptr, // Optional
		.pColorBlendState	 = &colorBlending,
		.pDynamicState		 = &dynamicState,
		.layout				 = m_PipelineLayout,
		.renderPass			 = m_RenderPass,
		.subpass			 = 0,
		.basePipelineHandle	 = VK_NULL_HANDLE, // Optional
		.basePipelineIndex	 = -1,			   // Optional
	};

	if (const auto result = vkCreateGraphicsPipelines(m_renderer->GetDeviceVK(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create pipeline. {}", result);
	}


	// don't leak stages
	for (auto& stage : stages)
	{
		vkDestroyShaderModule(m_renderer->GetDeviceVK(), stage.second, nullptr);
	}
}

//=================================================================================
void C_VkWindow::CreateRenderPass()
{
	const VkAttachmentDescription colorAttachment{
		.flags			= 0,
		.format			= m_SwapChainImageFormat,
		.samples		= VK_SAMPLE_COUNT_1_BIT,
		.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp		= VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED,	   // we don't care about previous state of image
		.finalLayout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, // we are going to present the results
	};

	// no subpassess - usable for post-process effects chained togather
	const VkAttachmentReference colorAttachmentRef{
		.attachment = 0,
		.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};

	const VkSubpassDescription subpass{
		.pipelineBindPoint	  = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments	  = &colorAttachmentRef,
	};

	const VkSubpassDependency dependency{
		.srcSubpass	   = VK_SUBPASS_EXTERNAL,
		.dstSubpass	   = 0,
		.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	};

	const VkRenderPassCreateInfo renderPassInfo{
		.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments	 = &colorAttachment,
		.subpassCount	 = 1,
		.pSubpasses		 = &subpass,
		.dependencyCount = 1,
		.pDependencies	 = &dependency,
	};

	if (const auto result = vkCreateRenderPass(m_renderer->GetDeviceVK(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create render pass. {}", result);
		return;
	}
}

//=================================================================================
void C_VkWindow::CreateFramebuffers()
{
	m_SwapChainFramebuffers.resize(m_SwapChainImagesViews.size());
	for (size_t i = 0; i < m_SwapChainImagesViews.size(); i++)
	{
		VkImageView attachments[] = {m_SwapChainImagesViews[i]};

		const VkFramebufferCreateInfo framebufferInfo{
			.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass		 = m_RenderPass,
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
	const VkCommandBufferAllocateInfo allocInfo{
		.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool		= m_CommandPool,
		.level				= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	if (const auto result = vkAllocateCommandBuffers(m_renderer->GetDeviceVK(), &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
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

	const VkClearValue			clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	const VkRenderPassBeginInfo renderPassInfo{
		.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass		 = m_RenderPass,
		.framebuffer	 = m_SwapChainFramebuffers[imageIndex],
		.renderArea		 = {{0, 0}, m_SwapChainExtent},
		.clearValueCount = 1,
		.pClearValues	 = &clearColor,
	};

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

	// we went for dynamic state of viewport and scissor
	const Renderer::C_Viewport viewport(0, 0, GetSize());
	const VkViewport		   vkViewport = TranslateViewport(viewport);
	vkCmdSetViewport(commandBuffer, 0, 1, &vkViewport);
	const VkRect2D scissor{.offset = {0, 0}, .extent = {viewport.GetResolution().x, viewport.GetResolution().y}};
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
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
	const VkSemaphoreCreateInfo semaphoreInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
	const VkFenceCreateInfo		fenceInfo{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};

	if (vkCreateSemaphore(m_renderer->GetDeviceVK(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore) != VK_SUCCESS
		|| vkCreateSemaphore(m_renderer->GetDeviceVK(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore) != VK_SUCCESS
		|| vkCreateFence(m_renderer->GetDeviceVK(), &fenceInfo, nullptr, &m_InFlightFence) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to record command buffer. {}");
	}
}

} // namespace GLEngine::VkRenderer
