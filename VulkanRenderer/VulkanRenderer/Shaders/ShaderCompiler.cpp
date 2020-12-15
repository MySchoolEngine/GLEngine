#include <VkRendererStdafx.h>

#include <VulkanRenderer/Shaders/ShaderCompiler.h>

namespace GLEngine::VkRenderer::Shaders {

//=================================================================================
C_ShaderCompiler::C_ShaderCompiler(VkDevice_T* device)
	: m_Device(device)
{

}

//=================================================================================
bool C_ShaderCompiler::compileShaderStageInternal(
	T_StageHandle& stage, 
	const std::filesystem::path& filepath, 
	const Renderer::E_ShaderStage shaderStage, 
	std::string& content)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = content.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(content.data());


	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create shader module! {}", filepath);
		return false;
	}

	return shaderModule;

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = shaderModule;
	fragShaderStageInfo.pName = filepath.generic_string().c_str();
}

}
