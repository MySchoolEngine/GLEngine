#pragma once

#include <Renderer/Definitions.h>

#include <Utils/Logging/LoggingMacros.h>

#include <vulkan/vulkan.h>

namespace GLEngine::VkRenderer {
//=================================================================================
// Shader stages
constexpr VkShaderStageFlagBits ShaderStageTypeToEnum(const Renderer::E_ShaderStage stage)
{
	switch (stage)
	{
	case Renderer::E_ShaderStage::Vertex:
		return VK_SHADER_STAGE_VERTEX_BIT;
	case Renderer::E_ShaderStage::Fragment:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	case Renderer::E_ShaderStage::Geometry:
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	case Renderer::E_ShaderStage::TesselationControl:
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	case Renderer::E_ShaderStage::TesselationEvaluation:
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	case Renderer::E_ShaderStage::Compute:
		return VK_SHADER_STAGE_COMPUTE_BIT;
	}
	CORE_LOG(E_Level::Error, E_Context::Render, "Unknown shader module");

	return VK_SHADER_STAGE_VERTEX_BIT;
}
} // namespace GLEngine::VkRenderer