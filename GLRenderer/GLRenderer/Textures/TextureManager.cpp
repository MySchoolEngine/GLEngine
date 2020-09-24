#include <GLRendererStdafx.h>

#include <GLRenderer/Textures/TextureManager.h>

#include <GLRenderer/Textures/Texture.h>

#include <GUI/GUIManager.h>
#include <GUI/GUIWindow.h>

#include <Renderer/Textures/TextureLoader.h>

#include <imgui.h>

namespace GLEngine::GLRenderer::Textures {
std::filesystem::path C_TextureManager::s_ErrorTextureFile = "Models/Error.bmp";

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

	Renderer::Textures::TextureLoader tl;
	Renderer::MeshData::Texture t;
	bool retval = tl.loadTexture(name.c_str(), t);
	if (!retval)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Could not load texture '{}'", name);
		return nullptr;
	}

	GLE_ASSERT(t.m_name.empty() == false, "There should be name from name: '{}'", name);
	return CreateTexture(t);
}

//=================================================================================
C_TextureManager::T_TexturePtr C_TextureManager::CreateTexture(const Renderer::MeshData::Texture& tex)
{
	auto texture = std::make_shared<Textures::C_Texture>(tex.m_name);
	texture->bind();
	texture->SetTexData2D(0, tex);
	texture->unbind();

	m_Textures[tex.m_name] = texture;
	return texture;
}

//=================================================================================
C_TextureManager::T_TexturePtr C_TextureManager::CreateEmptyTexture(const std::string& name)
{
	auto it = m_Textures.find(name);
	if (it != m_Textures.end())
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Texture with name '{}' already exists.", name);
		return nullptr;
	}

	auto texture = std::make_shared<Textures::C_Texture>(name);

	m_Textures[name] = texture;
	return texture;
}

//=================================================================================
void C_TextureManager::Clear()
{
	m_Textures.clear();
}

//=================================================================================
GUID C_TextureManager::SetupControls(GUI::C_GUIManager& guiMGR)
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
void C_TextureManager::DestroyControls(GUI::C_GUIManager& guiMGR)
{
	guiMGR.DestroyWindow(m_Window);
}

//=================================================================================
void C_TextureManager::ReloadTexture(const std::string& name, T_TexturePtr& texture)
{
	Renderer::Textures::TextureLoader tl;
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

//=================================================================================
C_TextureManager::T_TexturePtr C_TextureManager::GetErrorTexture()
{
	if (!m_ErrorTexture)
	{
		m_ErrorTexture = GetTexture(s_ErrorTextureFile.generic_string());
	}
	return m_ErrorTexture;
}

//=================================================================================
C_TextureManager::T_TexturePtr C_TextureManager::GetIdentityTexture()
{
	if (!m_IdentityTexture)
	{
		m_IdentityTexture = std::make_shared<C_Texture>("Identity texture");
		Renderer::MeshData::Texture t;
		t.height = t.width = 1;
		t.data = std::shared_ptr<unsigned char>(new unsigned char[4 * t.width * t.height]);
		t.data.get()[0] = 255;
		t.data.get()[1] = 255;
		t.data.get()[2] = 255;
		t.data.get()[3] = 0;
		m_IdentityTexture->bind();
		m_IdentityTexture->SetTexData2D(0, t);
		m_IdentityTexture->unbind();
	}
	return m_IdentityTexture;
}

}