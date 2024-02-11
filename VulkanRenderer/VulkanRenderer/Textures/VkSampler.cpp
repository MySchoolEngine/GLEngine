#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/Textures/VkSampler.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkSampler::C_VkSampler(const Renderer::SamplerDescriptor2D& desc)
	: desc(desc)
{
}

//=================================================================================
C_VkSampler::~C_VkSampler() = default;

//=================================================================================
const VkSampler& C_VkSampler::GetVkSampler() const
{
	return textureSampler;
}

}
