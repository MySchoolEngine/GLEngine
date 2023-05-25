#pragma once

#include <Renderer/Textures/DeviceTexture.h>

#include <Renderer/Resources/RenderResourceHandle.h>

namespace GLEngine::VkRenderer {
class C_VkTexture {
public:
	C_VkTexture(const Renderer::TextureDescriptor& desc);
	~C_VkTexture();

	void								SetSampler(Renderer::Handle<Renderer::Sampler> sampler);
	Renderer::Handle<Renderer::Sampler> GetSampler() const;

	VkImageView GetView() const;

private:
	Renderer::TextureDescriptor m_Desc;
	VkImage						textureImage;
	VkDeviceMemory				textureImageMemory;
	VkImageView					textureImageView;
	// add sampler
	Renderer::Handle<Renderer::Sampler> m_TextureSampler;

	friend class C_VkResourceManager;
	friend class C_VkRenderer; // todo remove
	friend class C_VkDevice;   // todo remove
};
} // namespace GLEngine::VkRenderer