#include <GLRendererStdafx.h>

#include <GLRenderer/Textures/TextureManager.h>

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureLoader.h>

#include <GLRenderer/ImGui/GUIManager.h>
#include <GLRenderer/GUI/GUIWindow.h>

namespace GLEngine::GLRenderer::Textures {

//=================================================================================
C_TextureManager::C_TextureManager()
	: m_Window(INVALID_GUID)
{

}

//=================================================================================
C_TextureManager& C_TextureManager::Instance()
{
	static C_TextureManager    instance; // Guaranteed to be destroyed.
							// Instantiated on first use.
	return instance;
}

//=================================================================================
C_TextureManager::T_TexturePtr C_TextureManager::GetTexture(const std::string& name)
{
	auto it = m_Textures.find(name);
	if (it != m_Textures.end())
	{
		return it->second;
	}

	Textures::TextureLoader tl;
	Renderer::MeshData::Texture t;
	bool retval = tl.loadTexture(name.c_str(), t);
	if (!retval)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Could not load texture '{}'", name);
		return nullptr;
	}

	auto texture = std::make_shared<Textures::C_Texture>(name);
	texture->bind();
	texture->SetTexData2D(0, t);
	texture->unbind();

	m_Textures[name] = texture;
	return texture;
}

//=================================================================================
void C_TextureManager::Clear()
{
	m_Textures.clear();
}

//=================================================================================
GUID C_TextureManager::SetupControls(ImGui::C_GUIManager& guiMGR)
{
	m_Window = guiMGR.CreateGUIWindow("Texture manager");
	auto* shaderMan = guiMGR.GetWindow(m_Window);

	m_TextureList = std::make_unique<GUI::C_LambdaPart>([&]() {

		for (auto& texture : m_Textures) {
			bool selected = false;
			::ImGui::Selectable(texture.first.c_str(), &selected);
			if (selected) {
				ReloadTexture(texture.first, texture.second);
			}
		}
		});

	shaderMan->AddComponent(*m_TextureList.get());

	return m_Window;
}

//=================================================================================
void C_TextureManager::DestroyControls(ImGui::C_GUIManager& guiMGR)
{
	guiMGR.DestroyWindow(m_Window);
}

//=================================================================================
void C_TextureManager::ReloadTexture(const std::string& name, T_TexturePtr& texture)
{
	Textures::TextureLoader tl;
	Renderer::MeshData::Texture t;
	bool retval = tl.loadTexture(name.c_str(), t);
	if (!retval)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Could not load texture '{}'", name);
		return;
	}

	texture->bind();
	texture->SetTexData2D(0, t);
	texture->unbind();
}

}