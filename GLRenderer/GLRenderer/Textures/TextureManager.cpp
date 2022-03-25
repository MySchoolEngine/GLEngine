#include <GLRendererStdafx.h>

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>

#include <Renderer/Descriptors/TextureDescriptor.h>
#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureView.h>

#include <GUI/GUIManager.h>
#include <GUI/GUIWindow.h>

#include <Core/Application.h>

#include <imgui.h>

namespace GLEngine::GLRenderer::Textures {
std::filesystem::path C_TextureManager::s_ErrorTextureFile = "Models/Error.bmp";

//=================================================================================
C_TextureManager::C_TextureManager()
	: m_Window(GUID::INVALID_GUID)
	, m_IdentityTexture(nullptr)
	, m_ErrorTexture(nullptr)
{
	// preload fallback textures
	{
		// Identity
		const Renderer::TextureDescriptor desc{"Identity texture", 1, 1, Renderer::E_TextureType::TEXTUE_2D, Renderer::E_TextureFormat::RGBA8i, false};

		m_IdentityTexture = std::make_shared<C_Texture>(desc);

		if (GetDevice().AllocateTexture(*m_IdentityTexture.get()))
		{
			Renderer::C_TextureViewStorageCPU<std::uint8_t> storage(1, 1, 4);
			Renderer::C_TextureView							view(&storage);
			view.Set<glm::vec4>(glm::ivec2(0, 0), glm::vec4(255, 255, 255, 0));
			m_IdentityTexture->SetTexData2D(0, &storage);
			m_IdentityTexture->m_IsPresentOnGPU = true;
		}
	}
	{
		// error texture
		Renderer::Textures::TextureLoader tl;
		const auto						  buffer = tl.loadTexture(s_ErrorTextureFile.generic_string().c_str());
		if (!buffer)
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Could not load texture '{}'", s_ErrorTextureFile.generic_string());
			return;
		}

		m_ErrorTexture = GetTexture(s_ErrorTextureFile.generic_string());
		const Renderer::TextureDescriptor desc{
			s_ErrorTextureFile.generic_string(), 
			buffer->GetDimensions().x,
			buffer->GetDimensions().y,
			Renderer::E_TextureType::TEXTUE_2D,
			Renderer::E_TextureFormat::RGBA8i, 
			false};

		m_ErrorTexture = std::make_shared<C_Texture>(desc);
		if (GetDevice().AllocateTexture(*m_ErrorTexture.get()))
		{
			ErrorCheck();
			m_ErrorTexture->SetTexData2D(0, buffer);
			ErrorCheck();
			m_ErrorTexture->m_IsPresentOnGPU = true;
		}
	}
}

//=================================================================================
C_TextureManager& C_TextureManager::Instance()
{
	static C_TextureManager instance; // Guaranteed to be destroyed.
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
	const auto						  buffer = tl.loadTexture(name.c_str());
	if (!buffer)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Could not load texture '{}'", name);
		return nullptr;
	}

	return CreateTexture(buffer, name);
}

//=================================================================================
C_TextureManager::T_TexturePtr C_TextureManager::CreateTexture(const Renderer::I_TextureViewStorage* tex, const std::string& name)
{
	// todo proper format:
	// 1] from metadata
	// 2] if not present than it should do immediate load - slow version
	const Renderer::TextureDescriptor desc{name, tex->GetDimensions().x, tex->GetDimensions().y, Renderer::E_TextureType::TEXTUE_2D, Renderer::E_TextureFormat::RGBA8i, true};
	auto							  texture = std::make_shared<Textures::C_Texture>(desc);
	if (GetDevice().AllocateTexture(*texture.get()))
	{
		texture->SetTexData2D(0, tex);
	}

	m_Textures[name] = texture;
	return texture;
}

//=================================================================================
C_TextureManager::T_TexturePtr C_TextureManager::CreateEmptyTexture(const std::string& name)
{
	// only used in shadow mapping. That should not go through texture manager.
	// this is candidate for deletion
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
	m_Window		= guiMGR.CreateGUIWindow("Texture manager");
	auto* shaderMan = guiMGR.GetWindow(m_Window);

	m_TextureList = std::make_unique<GUI::C_LambdaPart>([&]() {
		for (auto& texture : m_Textures)
		{
			bool selected = false;
			::ImGui::Selectable(texture.first.c_str(), &selected);
			ImGui::Image((void*)(intptr_t)(texture.second->GetTexture()), ImVec2(128, 128));
			if (selected)
			{
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
	// TODO: make it job thing
	Renderer::Textures::TextureLoader tl;
	auto							  tex = tl.loadTexture(name.c_str());
	if (!tex)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Could not load texture '{}'", name);
		return;
	}

	texture->SetTexData2D(0, tex);
}

//=================================================================================
C_TextureManager::T_TexturePtr C_TextureManager::GetErrorTexture()
{
	GLE_ASSERT(m_ErrorTexture, "This texture should be preloaded on engine start");
	return m_ErrorTexture;
}

//=================================================================================
C_TextureManager::T_TexturePtr C_TextureManager::GetIdentityTexture()
{
	GLE_ASSERT(m_IdentityTexture, "This texture should be preloaded on engine start");
	return m_IdentityTexture;
}

//=================================================================================
Renderer::I_Device& C_TextureManager::GetDevice()
{
	return Core::C_Application::Get().GetActiveRenderer().GetDevice();
}

} // namespace GLEngine::GLRenderer::Textures