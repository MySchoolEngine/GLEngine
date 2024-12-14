#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/Pipeline.h>
#include <VulkanRenderer/Shaders/ShaderCompiler.h>
#include <VulkanRenderer/VkDevice.h>
#include <VulkanRenderer/VkTypeHelpers.h>

#include <Renderer/Colours.h>

namespace GLEngine::VkRenderer {

//=================================================================================
void C_Pipeline::create(Renderer::I_Device& device, Renderer::PipelineDescriptor desc)
{
	std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

	const VkPipelineDynamicStateCreateInfo dynamicState{
		.sType			   = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pNext			   = nullptr,
		.flags			   = 0,
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates	   = dynamicStates.data(),
	};

	GLE_ASSERT(desc.bindingCount == desc.vertexInput.size() + desc.instanceInput.size(), "Wrong count of bindings.");

	std::vector<VkVertexInputBindingDescription> bindingDescriptions(desc.bindingCount);
	for (uint32_t i = 0; i < desc.bindingCount; ++i)
	{
		auto& bindingDesc	  = bindingDescriptions[i];
		bindingDesc.binding	  = i;
		bindingDesc.stride	  = 0;							 // will be updated
		bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // no instances yet
	}

	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	attributeDescriptions.reserve(desc.vertexInput.size());
	for (uint32_t i = 0; i < desc.vertexInput.size(); ++i)
	{
		const auto& attribDsec = desc.vertexInput[i];

		attributeDescriptions.emplace_back(VkVertexInputAttributeDescription{
			.location = i,
			.binding  = attribDsec.binding,
			.format	  = GetVkShaderDataFormat(attribDsec.type),
			.offset	  = bindingDescriptions[attribDsec.binding].stride,
		});
		bindingDescriptions[attribDsec.binding].stride += Renderer::ShaderDataTypeSize(attribDsec.type);
		// will be useful once I will start using instances
		GLE_ASSERT(bindingDescriptions[attribDsec.binding].inputRate == VK_VERTEX_INPUT_RATE_VERTEX, "Wrong input rate");
	}

	const VkPipelineVertexInputStateCreateInfo vertexInputInfo{
		.sType							 = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount	 = static_cast<uint32_t>(desc.bindingCount),
		.pVertexBindingDescriptions		 = bindingDescriptions.data(),
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
		.pVertexAttributeDescriptions	 = attributeDescriptions.data(), // Optional
	};

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
		.frontFace				 = VK_FRONT_FACE_COUNTER_CLOCKWISE,
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

	// could be from descriptor

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
	if (!loader.LoadAllStages(desc.shader, stages))
	{
		// error probably? End the run?
	}

	const VkDescriptorSetLayoutBinding uboLayoutBinding{
		.binding			= 0,
		.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount	= 1,
		.stageFlags			= VK_SHADER_STAGE_VERTEX_BIT,
		.pImmutableSamplers = nullptr,
	};

	const VkDescriptorSetLayoutBinding samplerLayoutBinding{
		.binding			= 1,
		.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount	= 1,
		.stageFlags			= VK_SHADER_STAGE_FRAGMENT_BIT,
		.pImmutableSamplers = nullptr,
	};

	std::array<const VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
	const VkDescriptorSetLayoutCreateInfo			  layoutInfo{
		.sType		  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings	  = bindings.data(),
	};

	if (const auto result = vkCreateDescriptorSetLayout(vkDevice.GetVkDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to create descriptor set layout. {}", result);
		return;
	}

	const VkPipelineLayoutCreateInfo pipelineLayoutInfo{
		.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext					= nullptr,
		.setLayoutCount			= 1,
		.pSetLayouts			= &m_DescriptorSetLayout,
		.pushConstantRangeCount = 0,
	};

	if (const auto result = vkCreatePipelineLayout(vkDevice.GetVkDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to create pipeline layout! {}", result);
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

	CreateRenderPass(device, desc);

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
void C_Pipeline::CreateRenderPass(Renderer::I_Device& device, Renderer::PipelineDescriptor desc)
{
	auto&						  vkDevice = static_cast<C_VkDevice&>(device);
	const VkAttachmentDescription colorAttachment{
		.flags			= 0,
		.format			= GetTextureFormat(desc.colorAttachementFormat),
		.samples		= VK_SAMPLE_COUNT_1_BIT,
		.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp		= VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED,	   // we don't care about previous state of image
		.finalLayout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, // we are going to present the results
	};

	// no subpassess - usable for post-process effects chained together
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
	vkDestroyDescriptorSetLayout(vkDevice.GetVkDevice(), m_DescriptorSetLayout, nullptr);
	vkDestroyPipeline(vkDevice.GetVkDevice(), m_GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(vkDevice.GetVkDevice(), m_PipelineLayout, nullptr);
	vkDestroyRenderPass(vkDevice.GetVkDevice(), m_RenderPass, nullptr);
}

} // namespace GLEngine::VkRenderer
