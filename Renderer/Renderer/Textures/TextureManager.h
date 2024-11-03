#pragma once

#include <Renderer/Textures/DeviceTexture.h>
#include <Renderer/Textures/TextureResource.h>

#include <Core/Resources/ResourceHandle.h>

// We need to allow even not yet loaded resources
// what about cloning?
namespace GLEngine::Renderer {
class I_Device;
class C_TextureManager {
public:
	// todo: Singleton for now
	static C_TextureManager& CreateInstance(I_Device& device);
	// this class is multi-tone and we want to avoid copy
	C_TextureManager(C_TextureManager const&) = delete;
	void operator=(C_TextureManager const&) = delete;

	[[nodiscard]] I_DeviceTexture* GetOrCreateTexture(const Core::ResourceHandle<Renderer::TextureResource> resource);

	void Update();

	[[nodiscard]] I_DeviceTexture* GetErrorTexture();
	// 1x1 px white texture
	[[nodiscard]] I_DeviceTexture* GetIdentityTexture();

private:
	C_TextureManager(I_Device& device);

	[[nodiscard]] std::shared_ptr<I_DeviceTexture> CreateTexture(Core::ResourceHandle<Renderer::TextureResource>);

	Renderer::I_Device&																			m_Device;
	//std::map<Core::ResourceHandle<Renderer::TextureResource>, std::shared_ptr<I_DeviceTexture>> m_Textures;
	std::vector<Core::ResourceHandle<Renderer::TextureResource>>								m_ToLoad;

	std::shared_ptr<I_DeviceTexture> m_ErrorTexture;
	std::shared_ptr<I_DeviceTexture> m_IdentityTexture;
	static std::filesystem::path	 s_ErrorTextureFile;
};
} // namespace GLEngine::Renderer
