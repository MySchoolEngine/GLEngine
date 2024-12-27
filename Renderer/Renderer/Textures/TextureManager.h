#pragma once

#include <Renderer/RendererApi.h>
#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/DeviceTexture.h>
#include <Renderer/Textures/TextureResource.h>

#include <Core/Resources/ResourceHandle.h>

// We need to allow even not yet loaded resources
// what about cloning?
namespace GLEngine::Renderer {
class I_Device;
class RENDERER_API_EXPORT C_TextureManager {
public:
	// todo: Singleton for now
	static C_TextureManager& CreateInstance(I_Device& device);
	// this class is multi-tone and we want to avoid copy
	C_TextureManager(C_TextureManager const&) = delete;
	void operator=(C_TextureManager const&)	  = delete;

	// virtual to be accessible from other modules
	[[nodiscard]] virtual Handle<Texture> GetOrCreateTexture(const Core::ResourceHandle<TextureResource> resource);
	[[nodiscard]] Handle<Texture>		  GetTexture(const Core::ResourceHandle<TextureResource>& resource) const;

	void Update();

	[[nodiscard]] I_DeviceTexture* GetErrorTexture();
	// 1x1 px white texture
	[[nodiscard]] I_DeviceTexture* GetIdentityTexture();

private:
	C_TextureManager(I_Device& device);

	[[nodiscard]] std::shared_ptr<I_DeviceTexture> CreateTexture(Core::ResourceHandle<TextureResource>);

	I_Device&																								   m_Device;
	std::map<Core::ResourceHandle<TextureResource>, Handle<Texture>, Core::ResourceHandleCmp<TextureResource>> m_Textures;
	std::vector<Core::ResourceHandle<TextureResource>>														   m_ToLoad;

	std::shared_ptr<I_DeviceTexture> m_ErrorTexture;
	std::shared_ptr<I_DeviceTexture> m_IdentityTexture;
	static std::filesystem::path	 s_ErrorTextureFile;
};
} // namespace GLEngine::Renderer
