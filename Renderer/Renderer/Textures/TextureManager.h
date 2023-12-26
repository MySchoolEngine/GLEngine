#pragma once

#include <Renderer/Textures/DeviceTexture.h>
#include <Renderer/Textures/TextureResource.h>

#include <Core/Resources/ResourceHandle.h>

namespace GLEngine::Renderer {
class I_Device;
class C_TextureManager {
public:
	// todo: Singleton for now
	static C_TextureManager& CreateInstance(I_Device& device);
	// this class is multi-tone and we want to avoid copy
	C_TextureManager(C_TextureManager const&) = delete;
	void operator=(C_TextureManager const&) = delete;

	/**
	 * Returns null if not loaded yet
	 */
	[[nodiscard]] I_DeviceTexture* GetTexture(const Core::ResourceHandle<Renderer::TextureResource>& resource);

	[[nodiscard]] I_DeviceTexture* CreateTexture(const Core::ResourceHandle<Renderer::TextureResource>& resource);

private:
	C_TextureManager(I_Device& device);


	Renderer::I_Device&															m_Device;
	//std::map<Core::ResourceHandle<Renderer::TextureResource>, I_DeviceTexture*> m_Textures;
};
} // namespace GLEngine::Renderer
