#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/Textures/VkTexture.h>

namespace GLEngine::VkRenderer {
//=================================================================================
C_VkTexture::C_VkTexture(const Renderer::TextureDescriptor& desc)
	: m_Desc(desc)
{
}

//=================================================================================
C_VkTexture::~C_VkTexture() = default;

}
