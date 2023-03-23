#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/Pipeline.h>
#include <VulkanRenderer/Shaders/ShaderCompiler.h>
#include <VulkanRenderer/VkDevice.h>
#include <VulkanRenderer/VkTypeHelpers.h>

namespace GLEngine::VkRenderer {

//=================================================================================
void C_Pipeline::create(Renderer::I_Device& device, Renderer::PipelineDescriptor desc, VkFormat swapCahinImageFormat)
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
		.topology				= GetVkDrawPrimitive(desc.primitiveType),
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
		.polygonMode			 = desc.wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL,
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
	auto&									 vkDevice = static_cast<C_VkDevice&>(device);
	C_ShaderCompiler						 compiler(vkDevice);
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

	CreateRenderPass(device, swapCahinImageFormat);

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

	if (const auto result = vkCreateGraphicsPipelines(vkDevice.GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create pipeline. {}", result);
	}


	// don't leak stages
	for (auto& stage : stages)
	{
		vkDestroyShaderModule(vkDevice.GetVkDevice(), stage.second, nullptr);
	}
}

//=================================================================================
void C_Pipeline::CreateRenderPass(Renderer::I_Device& device, VkFormat swapCahinImageFormat)
{
	auto&						  vkDevice = static_cast<C_VkDevice&>(device);
	const VkAttachmentDescription colorAttachment{
		.flags			= 0,
		.format			= swapCahinImageFormat,
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

	if (const auto result = vkCreateRenderPass(vkDevice.GetVkDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create render pass. {}", result);
		return;
	}
}

//=================================================================================
void C_Pipeline::destroy(Renderer::I_Device& device)
{
	auto& vkDevice = static_cast<C_VkDevice&>(device);
	vkDestroyPipeline(vkDevice.GetVkDevice(), m_GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(vkDevice.GetVkDevice(), m_PipelineLayout, nullptr);
	vkDestroyRenderPass(vkDevice.GetVkDevice(), m_RenderPass, nullptr);
}

} // namespace GLEngine::VkRenderer
