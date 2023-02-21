#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/Shaders/ShaderCompiler.h>
#include <VulkanRenderer/VkDevice.h>
#include <VulkanRenderer/VkTypeHelpers.h>

#include <Renderer/Shaders/ShaderPreprocessor.h>
#include <Renderer/Viewport.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_ShaderCompiler::C_ShaderCompiler(C_VkDevice& device, bool preprocessorOutput /*= false*/)
	: m_PreprocessorOutput(preprocessorOutput)
	, m_Device(device)
{
}
//=================================================================================
bool C_ShaderCompiler::compileShaderStageInternal(T_StageHandle&				stage,
												  const std::filesystem::path&	filepath,
												  const Renderer::E_ShaderStage shaderStage,
												  std::vector<char>&			content)
{
	if (filepath.extension() == "xml")
	{
		// dead path now
		std::string								strContent(content.begin(), content.end());
		Renderer::Shaders::C_ShaderPreprocessor preproces(nullptr); // TODO
		strContent = preproces.PreprocessFile(strContent, filepath.parent_path());
		if (m_PreprocessorOutput)
		{
			std::ofstream				debugOutput;
			const std::filesystem::path debugPath("obj/" + filepath.generic_string() + ".o");
			const auto					debugDirectory = debugPath.parent_path();
			if (!std::filesystem::exists(debugDirectory))
			{
				if (!std::filesystem::create_directories(debugDirectory))
				{
					CORE_LOG(E_Level::Error, E_Context::Render, "Cannot create debug output directory");
				}
			}
			debugOutput.open(debugPath);
			if (!debugOutput.is_open())
			{
				CORE_LOG(E_Level::Error, E_Context::Render, "Cannot open file for debug output");
			}
			debugOutput << strContent;
			debugOutput.flush();
			debugOutput.close();
		}

		if (!preproces.WasSuccessful())
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Preprocessing of file '{}' was unsuccessful", filepath);
			return false;
		}

		auto paths = preproces.GetTouchedPaths();
		m_TouchedFiles.insert(m_TouchedFiles.end(), paths.begin(), paths.end());
	}
	else
	{
		const VkShaderModuleCreateInfo createInfo{.sType	= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
												  .pNext	= nullptr,
												  .flags	= 0,
												  .codeSize = content.size(),
												  .pCode	= reinterpret_cast<const uint32_t*>(content.data())};

		if (const auto result = vkCreateShaderModule(m_Device.GetVkDevice(), &createInfo, nullptr, &stage) != VK_SUCCESS)
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create shader module {}: {}", filepath, result);
			return false;
		}
	}

	return true;
}

//=================================================================================
bool C_ShaderCompiler::linkProgram(VkPipelineLayout&													  pipelineLayout,
								   const std::vector<std::pair<Renderer::E_ShaderStage, VkShaderModule>>& stages,
								   const std::string&													  name,
								   const Renderer::C_Viewport&											  viewport)
{
	bool							 bResult	  = true;
	VkPipelineShaderStageCreateInfo* shaderStages = new VkPipelineShaderStageCreateInfo[stages.size()];
	for (const auto & stage : stages)
	{
		const VkPipelineShaderStageCreateInfo fragShaderStageInfo{
			.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext	= nullptr,
			.flags	= 0,
			.stage	= GetVkShaderStage(stage.first),
			.module = stage.second,
			.pName	= name.c_str(),
		};
	}

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

	const VkPipelineLayoutCreateInfo pipelineLayoutInfo{
		.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext					= nullptr,
		.setLayoutCount			= 0,
		.pushConstantRangeCount = 0,
	};

	if (const auto result = vkCreatePipelineLayout(m_Device.GetVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to create pipeline layout! {}", result);
		bResult = false;
	}

	// don't leak stages
	for (auto& stage : stages)
	{
		vkDestroyShaderModule(m_Device.GetVkDevice(), stage.second, nullptr);
	}
	return bResult;
}

//=================================================================================
void C_ShaderCompiler::ReleaseStage(T_StageHandle& stage)
{
	vkDestroyShaderModule(m_Device.GetVkDevice(), stage, nullptr);
}

} // namespace GLEngine::VkRenderer
