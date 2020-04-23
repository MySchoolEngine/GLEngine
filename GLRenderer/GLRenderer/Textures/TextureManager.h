#pragma once

#include <GLRenderer/GUI/LambdaPart.h>

namespace GLEngine::GLRenderer::ImGui
{
class C_GUIManager;
}

namespace GLEngine::GLRenderer::Textures {
class C_Texture;

class C_TextureManager
{
	using T_TexturePtr = std::shared_ptr<C_Texture>;
public:
	//Singleton stuff
	C_TextureManager(C_TextureManager const&) = delete;
	void operator=(C_TextureManager const&) = delete;
	[[nodiscard]] static C_TextureManager& Instance();

	[[nodiscard]] T_TexturePtr GetTexture(const std::string& name);

	void Clear();

	[[nodiscard]] GUID SetupControls(ImGui::C_GUIManager& guiMGR);
	void DestroyControls(ImGui::C_GUIManager& guiMGR);

private:
	C_TextureManager();

	using T_TextureMap = std::map<std::string, T_TexturePtr>;
	T_TextureMap	m_Textures;

	void ReloadTexture(const std::string& name, T_TexturePtr& texture);

	GUID																m_Window;
	std::unique_ptr<GUI::C_LambdaPart>	m_TextureList;
};
}