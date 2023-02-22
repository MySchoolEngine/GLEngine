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
}

//=================================================================================
C_VkWindow::~C_VkWindow()
{
	DestroySwapchain();
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	vkDestroyPipeline(m_renderer->GetDeviceVK(), m_GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_renderer->GetDeviceVK(), m_PipelineLayout, nullptr);
	vkDestroyRenderPass(m_renderer->GetDeviceVK(), m_RenderPass, nullptr);
	m_renderer.reset(nullptr);
};

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
	DestroySwapchain();
	CreateSwapChain();
	vkDestroyPipeline(m_renderer->GetDeviceVK(), m_GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_renderer->GetDeviceVK(), m_PipelineLayout, nullptr);
	vkDestroyRenderPass(m_renderer->GetDeviceVK(), m_RenderPass, nullptr);
	CreatePipeline();

	return true;
}

//=================================================================================
void C_VkWindow::CreatePipeline()
{
	const Renderer::C_Viewport	viewport(0, 0, GetSize());
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

	const VkViewport vKViewport = TranslateViewport(viewport);

	const VkRect2D scissor{.offset = {0, 0}, .extent = {viewport.GetResolution().x, viewport.GetResolution().y}};

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

	VkRenderPassCreateInfo renderPassInfo{
		.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments	 = &colorAttachment,
		.subpassCount	 = 1,
		.pSubpasses		 = &subpass,
	};

	if (const auto result = vkCreateRenderPass(m_renderer->GetDeviceVK(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create render pass. {}", result);
		return;
	}
}

} // namespace GLEngine::VkRenderer
