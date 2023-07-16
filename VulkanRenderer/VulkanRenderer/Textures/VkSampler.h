#pragma once

#include <Renderer/Textures/TextureSampler.h>

namespace GLEngine::VkRenderer {
class C_VkSampler {
public:
	C_VkSampler(const Renderer::SamplerDescriptor2D& desc);
	~C_VkSampler();

	const VkSampler& GetVkSampler() const;// todo

private:
	Renderer::SamplerDescriptor2D desc;
	VkSampler textureSampler;

	friend class C_VkResourceManager;
	friend class C_VkRenderer; // todo remove
	friend class C_VkDevice;   // todo remove
};
} // namespace GLEngine::VkRenderer
