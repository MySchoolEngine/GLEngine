#pragma once

#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/DeviceTexture.h>

namespace GLEngine::VkRenderer {
class C_VkTexture {
public:
	C_VkTexture(const Renderer::TextureDescriptor& desc);
	~C_VkTexture();

	void								SetSampler(Renderer::Handle<Renderer::Sampler> sampler);
	Renderer::Handle<Renderer::Sampler> GetSampler() const;

	const Renderer::TextureDescriptor& GetDesc() const { return m_Desc; }

	VkImageView GetView() const;

	void			SetGUIHandle(VkDescriptorSet guiHandle) { GUIHandle = guiHandle; }
	VkDescriptorSet GetGUIHandle() const { return GUIHandle; }

private:
	Renderer::TextureDescriptor m_Desc;
	VkImage						textureImage;
	VkDeviceMemory				textureImageMemory;
	VkImageView					textureImageView;
	// add sampler
	Renderer::Handle<Renderer::Sampler> m_TextureSampler;

	VkDescriptorSet GUIHandle = VK_NULL_HANDLE;

	friend class C_VkResourceManager;
	friend class C_VkRenderer; // todo remove
	friend class C_VkDevice;   // todo remove
};
} // namespace GLEngine::VkRenderer