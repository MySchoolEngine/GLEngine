#include <VulkanRendererStdAfx.h>

#include <VulkanRenderer/VkTypeHelpers.h>

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
VkBufferUsageFlagBits GetVkBufferUsage(const Renderer::E_BufferType type)
{
	switch (type)
	{
	case Renderer::VertexAttribute:
		return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	case Renderer::Uniform:
		return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	}
	GLE_ASSERT(false, "Unknown type");
	return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
}

} // namespace GLEngine::VkRenderer
