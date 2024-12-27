#pragma once

#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>

#include <GUI/LambdaPart.h>


namespace GLEngine::Renderer {
class I_Device;
class I_Renderer;
} // namespace GLEngine::Renderer
namespace GLEngine::GUI {
class C_GUIManager;
}

namespace GLEngine::GLRenderer::Textures {
class C_Texture;

class C_TextureManager {
	using T_TexturePtr = std::shared_ptr<C_Texture>;

public:
	// Singleton stuff
	// TODO: make it multi-tone as you would want to create one texture manager for each API
	C_TextureManager(C_TextureManager const&) = delete;
	void operator=(C_TextureManager const&)	  = delete;

	// Dependency injection here should allow me one day move away from singleton
	[[nodiscard]] static C_TextureManager& Instance(Renderer::I_Renderer* renderer = nullptr, Renderer::I_Device* device = nullptr);

	[[nodiscard]] T_TexturePtr GetTexture(const std::string& name);
	[[nodiscard]] T_TexturePtr CreateTexture(const Renderer::I_TextureViewStorage* tex, const std::string& name);

	void Clear();

	[[nodiscard]] GUID SetupControls(GUI::C_GUIManager& guiMGR);
	void			   DestroyControls(GUI::C_GUIManager& guiMGR);


	[[nodiscard]] T_TexturePtr GetErrorTexture();
	// 1x1 px white texture
	[[nodiscard]] C_Texture*						  GetIdentityTexture();
	[[nodiscard]] Renderer::Handle<Renderer::Texture> GetIdentityTextureHandle();

private:
	explicit C_TextureManager(Renderer::I_Renderer& renderer, Renderer::I_Device& device);

	using T_TextureMap = std::map<std::string, T_TexturePtr>;
	T_TextureMap m_Textures;

	Renderer::C_TextureViewStorageCPU<float> m_IdentityStorage; // needs to live somewhere to survive until upload
	T_TexturePtr							 m_ErrorTexture;
	Renderer::Handle<Renderer::Texture>		 m_IdentityTexture;
	static std::filesystem::path			 s_ErrorTextureFile;

	void ReloadTexture(const std::string& name, T_TexturePtr& texture);

	Renderer::I_Device& m_Device;

	[[nodiscard]] Renderer::I_Device& GetDevice();

	GUID							   m_Window;
	std::unique_ptr<GUI::C_LambdaPart> m_TextureList;
};
} // namespace GLEngine::GLRenderer::Textures