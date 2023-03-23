#include <VulkanRendererStdAfx.h>

#include <VulkanRenderer/VkTypeHelpers.h>

#include <Renderer/Viewport.h>

namespace GLEngine::VkRenderer {

//=================================================================================
VkFormat GetVkInternalFormat(const Renderer::E_TextureFormat format)
{
	switch (format)
	{
	case Renderer::E_TextureFormat::RGBA32f:
		return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
	case Renderer::E_TextureFormat::RGB32f:
		return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
	case Renderer::E_TextureFormat::RG32f:
		return VkFormat::VK_FORMAT_R32G32_SFLOAT;
	case Renderer::E_TextureFormat::R32f:
		return VkFormat::VK_FORMAT_R32_SFLOAT;
	case Renderer::E_TextureFormat::RGBA16f:
		return VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT;
	case Renderer::E_TextureFormat::RGB16f:
		return VkFormat::VK_FORMAT_R16G16B16_SFLOAT;
	case Renderer::E_TextureFormat::RG16f:
		return VkFormat::VK_FORMAT_R16G16_SFLOAT;
	case Renderer::E_TextureFormat::R16f:
		return VkFormat::VK_FORMAT_R16_SFLOAT;
	case Renderer::E_TextureFormat::RGBA32i:
		return VkFormat::VK_FORMAT_R32G32B32A32_SINT;
	case Renderer::E_TextureFormat::RGB32i:
		return VkFormat::VK_FORMAT_R32G32B32_SINT;
	case Renderer::E_TextureFormat::RG32i:
		return VkFormat::VK_FORMAT_R32G32_SINT;
	case Renderer::E_TextureFormat::R32i:
		return VkFormat::VK_FORMAT_R32_SINT;
	case Renderer::E_TextureFormat::RGBA16i:
		return VkFormat::VK_FORMAT_R16G16B16A16_SINT;
	case Renderer::E_TextureFormat::RGB16i:
		return VkFormat::VK_FORMAT_R16G16B16_SINT;
	case Renderer::E_TextureFormat::RG16i:
		return VkFormat::VK_FORMAT_R16G16_SINT;
	case Renderer::E_TextureFormat::R16i:
		return VkFormat::VK_FORMAT_R16_SINT;
	case Renderer::E_TextureFormat::RGBA8i:
		return VkFormat::VK_FORMAT_R8G8B8A8_SINT;
	case Renderer::E_TextureFormat::RGB8i:
		return VkFormat::VK_FORMAT_R8G8B8_SINT;
	case Renderer::E_TextureFormat::RG8i:
		return VkFormat::VK_FORMAT_R8G8_SINT;
	case Renderer::E_TextureFormat::R8i:
		return VkFormat::VK_FORMAT_R8_SINT;
	case Renderer::E_TextureFormat::D32f:
	case Renderer::E_TextureFormat::D24S8:
	case Renderer::E_TextureFormat::D24:
	case Renderer::E_TextureFormat::D16:
	default:
		CORE_LOG(E_Level::Error, E_Context::Render, "Unknown texture format");
		break;
	}
	return VkFormat::VK_FORMAT_R8_SINT;
}

//=================================================================================
VkShaderStageFlagBits GetVkShaderStage(Renderer::E_ShaderStage stage)
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
	default:
		break;
	}

	GLE_ERROR("Unknown shader stage.")
}

//=================================================================================
VkViewport TranslateViewport(const Renderer::C_Viewport& viewport)
{
	return {
		.x		  = static_cast<float>(viewport.GetCoordinates().x),
		.y		  = static_cast<float>(viewport.GetCoordinates().y),
		.width	  = static_cast<float>(viewport.GetResolution().x),
		.height	  = static_cast<float>(viewport.GetResolution().y),
		.minDepth = 0.f,
		.maxDepth = 1.f,
	};
}

//=================================================================================
VkPrimitiveTopology GetVkDrawPrimitive(const Renderer::E_RenderPrimitives renderPrimitive)
{
	switch (renderPrimitive)
	{
	case Renderer::E_RenderPrimitives::TriangleList:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case Renderer::E_RenderPrimitives::LineList:
		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case Renderer::E_RenderPrimitives::PointList:
		return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	default:
		GLE_ERROR("Unknown render primitive");
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}
}

} // namespace GLEngine::VkRenderer
