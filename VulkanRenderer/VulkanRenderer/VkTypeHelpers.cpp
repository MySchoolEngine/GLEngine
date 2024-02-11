#include <VulkanRendererStdafx.h>

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
VkFilter GetVkInternalFormat(const Renderer::E_TextureFilter format)
{
	switch (format)
	{
	case Renderer::E_TextureFilter::Linear:
		return VK_FILTER_LINEAR;
	case Renderer::E_TextureFilter::Nearest:
		return VK_FILTER_NEAREST;
	default:
		GLE_ERROR("Unknown filter");
		return VK_FILTER_LINEAR;
	}
}

//=================================================================================
VkSamplerAddressMode GetVkInternalFormat(const Renderer::E_WrapFunction wrapFunction)
{
	switch (wrapFunction)
	{
	case Renderer::E_WrapFunction::ClampToEdge:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case Renderer::E_WrapFunction::ClampToBorder:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	case Renderer::E_WrapFunction::MirroredRepeat:
		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	case Renderer::E_WrapFunction::Repeat:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	default:
		GLE_ERROR("Unknown wrap function");
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	}
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

	GLE_ERROR("Unknown shader stage.");
	return VK_SHADER_STAGE_VERTEX_BIT;
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

//=================================================================================
VkFormat GetVkShaderDataFormat(const Renderer::E_ShaderDataType format)
{
	switch (format)
	{
	case Renderer::E_ShaderDataType::Float:
		return VK_FORMAT_R32_SFLOAT;
	case Renderer::E_ShaderDataType::Vec2:
		return VK_FORMAT_R32G32_SFLOAT;
	case Renderer::E_ShaderDataType::Vec3:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case Renderer::E_ShaderDataType::Vec4:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	default:
		GLE_ERROR("Unknown shader data format");
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	}
}

//=================================================================================
VkBufferUsageFlags GetBufferType(const Renderer::E_BufferType bufferType)
{
	switch (bufferType)
	{
	case Renderer::E_BufferType::Vertex:
		return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	case Renderer::E_BufferType::Index:
		return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	case Renderer::E_BufferType::Uniform:
		return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	default:
		GLE_ERROR("Unknown buffer type");
		return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	}
}

//=================================================================================
VkFormat GetTextureFormat(const Renderer::E_TextureFormat textureFormat)
{
	switch (textureFormat)
	{
		// float formats
	case Renderer::E_TextureFormat::RGBA32f:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case Renderer::E_TextureFormat::RGB32f:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case Renderer::E_TextureFormat::RG32f:
		return VK_FORMAT_R32G32_SFLOAT;
	case Renderer::E_TextureFormat::R32f:
		return VK_FORMAT_R32_SFLOAT;
	case Renderer::E_TextureFormat::RGBA16f:
		return VK_FORMAT_R16G16B16A16_SFLOAT;
	case Renderer::E_TextureFormat::RGB16f:
		return VK_FORMAT_R16G16B16_SFLOAT;
	case Renderer::E_TextureFormat::RG16f:
		return VK_FORMAT_R16G16_SFLOAT;
	case Renderer::E_TextureFormat::R16f:
		return VK_FORMAT_R16_SFLOAT;

	// integer formats 32 bits
	case Renderer::E_TextureFormat::RGBA32i:
		return VK_FORMAT_R32G32B32A32_SINT;
	case Renderer::E_TextureFormat::RGB32i:
		return VK_FORMAT_R32G32B32_SINT;
	case Renderer::E_TextureFormat::RG32i:
		return VK_FORMAT_R32G32_SINT;
	case Renderer::E_TextureFormat::R32i:
		return VK_FORMAT_R32_SINT;

	// integer formats 16 bits
	case Renderer::E_TextureFormat::RGBA16i:
		return VK_FORMAT_R16G16B16A16_SINT;
	case Renderer::E_TextureFormat::RGB16i:
		return VK_FORMAT_R16G16B16_SINT;
	case Renderer::E_TextureFormat::RG16i:
		return VK_FORMAT_R16G16_SINT;
	case Renderer::E_TextureFormat::R16i:
		return VK_FORMAT_R16_SINT;

	// integer formats 8 bits
	case Renderer::E_TextureFormat::RGBA8i:
		return VK_FORMAT_R8G8B8A8_SINT;
	case Renderer::E_TextureFormat::RGB8i:
		return VK_FORMAT_R8G8B8_SINT;
	case Renderer::E_TextureFormat::RG8i:
		return VK_FORMAT_R8G8_SINT;
	case Renderer::E_TextureFormat::R8i:
		return VK_FORMAT_R8_SINT;

	// integer formats 8 bits srgb
	case Renderer::E_TextureFormat::RGBA8isrgb:
		return VK_FORMAT_R8G8B8A8_SRGB;
	case Renderer::E_TextureFormat::BGRA8isrgb:
		return VK_FORMAT_B8G8R8A8_SRGB;
	case Renderer::E_TextureFormat::RGB8isrgb:
		return VK_FORMAT_R8G8B8_SRGB;
	case Renderer::E_TextureFormat::RG8isrgb:
		return VK_FORMAT_R8G8_SRGB;
	case Renderer::E_TextureFormat::R8isrgb:
		return VK_FORMAT_R8_SRGB;

	// integer formats depths
	case Renderer::E_TextureFormat::D24S8:
		return VK_FORMAT_D24_UNORM_S8_UINT;
	case Renderer::E_TextureFormat::D32f:
		return VK_FORMAT_D32_SFLOAT;
	case Renderer::E_TextureFormat::D24:
		return VK_FORMAT_D24_UNORM_S8_UINT;
	case Renderer::E_TextureFormat::D16:
		return VK_FORMAT_D16_UNORM;
	}
	GLE_ERROR("Unknown format!");
	return VK_FORMAT_R8G8B8A8_UINT;
}

//=================================================================================
Renderer::E_TextureFormat GetTextureFormat(VkFormat textureFormat)
{
	switch (textureFormat)
	{
		using enum Renderer::E_TextureFormat;
		// float formats
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return RGBA32f;
	case VK_FORMAT_R32G32B32_SFLOAT:
		return RGB32f;
	case VK_FORMAT_R32G32_SFLOAT:
		return RG32f;
	case VK_FORMAT_R32_SFLOAT:
		return R32f;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		return RGBA16f;
	case VK_FORMAT_R16G16B16_SFLOAT:
		return RGB16f;
	case VK_FORMAT_R16G16_SFLOAT:
		return RG16f;
	case VK_FORMAT_R16_SFLOAT:
		return R16f;

	// integer formats 32 bits
	case VK_FORMAT_R32G32B32A32_SINT:
		return RGBA32i;
	case VK_FORMAT_R32G32B32_SINT:
		return RGB32i;
	case VK_FORMAT_R32G32_SINT:
		return RG32i;
	case VK_FORMAT_R32_SINT:
		return R32i;

	// integer formats 16 bits
	case VK_FORMAT_R16G16B16A16_SINT:
		return RGBA16i;
	case VK_FORMAT_R16G16B16_SINT:
		return RGB16i;
	case VK_FORMAT_R16G16_SINT:
		return RG16i;
	case VK_FORMAT_R16_SINT:
		return R16i;

	// integer formats 8 bits
	case VK_FORMAT_R8G8B8A8_SINT:
		return RGBA8i;
	case VK_FORMAT_R8G8B8_SINT:
		return RGB8i;
	case VK_FORMAT_R8G8_SINT:
		return RG8i;
	case VK_FORMAT_R8_SINT:
		return R8i;

	// integer formats 8 bits srgb
	case VK_FORMAT_R8G8B8A8_SRGB:
		return RGBA8isrgb;
	case VK_FORMAT_B8G8R8A8_SRGB:
		return BGRA8isrgb;
	case VK_FORMAT_R8G8B8_SRGB:
		return RGB8isrgb;
	case VK_FORMAT_R8G8_SRGB:
		return RG8isrgb;
	case VK_FORMAT_R8_SRGB:
		return R8isrgb;

	// integer formats depths
	case VK_FORMAT_D24_UNORM_S8_UINT:
		return D24S8; //< Prefer this over just D24
		//return D24;
	case VK_FORMAT_D32_SFLOAT:
		return D32f;
	case VK_FORMAT_D16_UNORM:
		return D16;
	}
	GLE_ERROR("Unknown format!");
	return Renderer::E_TextureFormat::RGBA8i;
}

//=================================================================================
VkImageType GetTextureType(const Renderer::E_TextureType textureType)
{
	switch (textureType)
	{
	case Renderer::E_TextureType::TEXTURE_2D:
		return VK_IMAGE_TYPE_2D;
	case Renderer::E_TextureType::TEXTURE_2D_ARRAY:
		return VK_IMAGE_TYPE_2D;
	case Renderer::E_TextureType::CUBE_MAP:
		break;
	}
	GLE_ERROR("Unknown type!");
	return VK_IMAGE_TYPE_2D;
}

//=================================================================================
VkSamplerMipmapMode GetVkInternalMipMapFilter(const Renderer::E_TextureFilter format)
{
	switch (format)
	{
	case Renderer::E_TextureFilter::Linear:
		return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
	case Renderer::E_TextureFilter::Nearest:
		return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
	case Renderer::E_TextureFilter::NearestMipMapNearest:
		return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
	case Renderer::E_TextureFilter::LinearMipMapNearest:
		return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
	case Renderer::E_TextureFilter::LinearMipMapLinear:
		return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
	case Renderer::E_TextureFilter::NearestMipMapLinear:
		return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
	default:
		break;

	}
	GLE_ERROR("Unknown mip map filter!");
	return VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
}

} // namespace GLEngine::VkRenderer
