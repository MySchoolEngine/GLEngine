#include <GLRendererStdafx.h>

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>

#include <Renderer/Descriptors/TextureDescriptor.h>
#include <Renderer/IDevice.h>
#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureView.h>

#include <GUI/GUIManager.h>
#include <GUI/GUIWindow.h>

#include <imgui.h>

namespace GLEngine::GLRenderer::Textures {
std::filesystem::path C_TextureManager::s_ErrorTextureFile = "Models/Error.bmp";

//=================================================================================
C_TextureManager::C_TextureManager(Renderer::I_Device& device)
	: m_Window(GUID::INVALID_GUID)
	, m_IdentityTexture(nullptr)
	, m_ErrorTexture(nullptr)
	, m_Device(device)
{
	// preload fallback textures
	{
		// Identity
		const Renderer::TextureDescriptor desc{"Identity texture", 1, 1, Renderer::E_TextureType::TEXTURE_2D, Renderer::E_TextureFormat::RGBA16f, false};

		m_IdentityTexture = std::make_shared<C_Texture>(desc);

		if (GetDevice().AllocateTexture(*m_IdentityTexture.get()))
		{
			Renderer::C_TextureViewStorageCPU<float> storage(1, 1, 4);
			Renderer::C_TextureView							view(&storage);
			view.Set<glm::vec4>(glm::ivec2(0, 0), glm::vec4(255, 255, 255, 0));
			m_IdentityTexture->SetTexData2D(0, &storage);
		}
		m_IdentityTexture->CreateHandle();
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

		const Renderer::TextureDescriptor desc{
			s_ErrorTextureFile.generic_string(), 
			buffer->GetDimensions().x,
			buffer->GetDimensions().y,
			Renderer::E_TextureType::TEXTURE_2D,
			Renderer::E_TextureFormat::RGBA32f, 
			false,
			9};

		m_ErrorTexture = std::make_shared<C_Texture>(desc);
		if (GetDevice().AllocateTexture(*m_ErrorTexture.get()))
		{
			m_ErrorTexture->SetWrap(Renderer::E_WrapFunction::Repeat, Renderer::E_WrapFunction::Repeat);
			m_ErrorTexture->SetFilter(Renderer::E_TextureFilter::LinearMipMapLinear, Renderer::E_TextureFilter::Linear);
			m_ErrorTexture->SetTexData2D(0, buffer.get());
		}
		m_ErrorTexture->GenerateMipMaps();
	}
}

//=================================================================================
C_TextureManager& C_TextureManager::Instance(Renderer::I_Device* device)
{
	static C_TextureManager instance(*device); // Guaranteed to be destroyed.
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

	return CreateTexture(buffer.get(), name);
}

//=================================================================================
C_TextureManager::T_TexturePtr C_TextureManager::CreateTexture(const Renderer::I_TextureViewStorage* tex, const std::string& name)
{
	// todo proper format:
	// 1] from metadata
	// 2] if not present than it should do immediate load - slow version
	const Renderer::TextureDescriptor desc{name,
										   tex->GetDimensions().x,
										   tex->GetDimensions().y,
										   Renderer::E_TextureType::TEXTURE_2D,
										   Renderer::GetClosestFormat(tex->GetChannels(), !Renderer::IsIntegral(tex->GetStorageType())),
										   true,
										   10};
	auto							  texture = std::make_shared<Textures::C_Texture>(desc);
	if (GetDevice().AllocateTexture(*texture.get()))
	{
		texture->SetTexData2D(0, tex);
	}

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
		return false; // TODO?
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

	texture->SetTexData2D(0, tex.get());
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
	return m_Device;
}

} // namespace GLEngine::GLRenderer::Textures