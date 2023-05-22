#pragma once

#include <Renderer/Textures/DeviceTexture.h>

namespace GLEngine::VkRenderer {
class C_VkTexture {
public:
	C_VkTexture(const Renderer::TextureDescriptor& desc);
	~C_VkTexture();

private:
	Renderer::TextureDescriptor m_Desc;
	VkImage						textureImage;
	VkDeviceMemory				textureImageMemory;

	friend class C_VkResourceManager;
	friend class C_VkRenderer; // todo remove
	friend class C_VkDevice;   // todo remove
};
} // namespace GLEngine::VkRenderer