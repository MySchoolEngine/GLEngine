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

//=================================================================================
void C_VkTexture::SetSampler(Renderer::Handle<Renderer::Sampler> sampler)
{
	m_TextureSampler = sampler;
}

//=================================================================================
Renderer::Handle<GLEngine::Renderer::Sampler> C_VkTexture::GetSampler() const
{
	return m_TextureSampler;
}

}
