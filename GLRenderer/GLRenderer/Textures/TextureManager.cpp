#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>

#include <Renderer/Descriptors/TextureDescriptor.h>
#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Resources/ResourceManager.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>
#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureView.h>

#include <GUI/GUIManager.h>
#include <GUI/GUIWindow.h>

#include <Core/Application.h>

#include <imgui.h>

namespace GLEngine::GLRenderer::Textures {
std::filesystem::path C_TextureManager::s_ErrorTextureFile = "Models/Error.bmp";

//=================================================================================
C_TextureManager::C_TextureManager(Renderer::I_Renderer& renderer, Renderer::I_Device& device)
	: m_IdentityStorage(1, 1, 4)
	, m_ErrorTexture(nullptr)
	, m_Device(device)
	, m_Window(GUID::INVALID_GUID)
{
	// preload fallback textures
	{
		auto& glRM = dynamic_cast<C_OGLRenderer&>(renderer).GetRMGR();
		// Identity
		const Renderer::TextureDescriptor desc{.name		  = "Identity texture",
											   .width		  = 1,
											   .height		  = 1,
											   .type		  = Renderer::E_TextureType::TEXTURE_2D,
											   .format		  = Renderer::E_TextureFormat::RGBA16f,
											   .m_bStreamable = false};

		m_IdentityTexture = glRM.createTexture(desc);

		if (m_IdentityTexture)
		{
			Renderer::C_TextureView view(&m_IdentityStorage);
			view.Set<glm::vec4>(glm::ivec2(0, 0), glm::vec4(255, 255, 255, 0));
			renderer.SetTextureData(m_IdentityTexture, m_IdentityStorage);
		}
		auto* glIdentityTexture = glRM.GetTexture(m_IdentityTexture);
		GLE_TODO("27-12-2024", "RohacekD", "This is nasty hack. Calling this here woudl cause getting error texture (Identity wasn't uploaded to GPU yet)");
		renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([glIdentityTexture]() { glIdentityTexture->CreateHandle(); }, "Create identity handle"));
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

		const Renderer::TextureDescriptor desc{.name		  = s_ErrorTextureFile.generic_string(),
											   .width		  = buffer->GetDimensions().x,
											   .height		  = buffer->GetDimensions().y,
											   .type		  = Renderer::E_TextureType::TEXTURE_2D,
											   .format		  = Renderer::E_TextureFormat::RGBA32f,
											   .m_bStreamable = false,
											   .m_Levels	  = 9};

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
C_TextureManager& C_TextureManager::Instance(Renderer::I_Renderer* renderer, Renderer::I_Device* device)
{
	static C_TextureManager instance(*renderer, *device); // Guaranteed to be destroyed.
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
	const Renderer::TextureDescriptor desc{.name		  = name,
										   .width		  = tex->GetDimensions().x,
										   .height		  = tex->GetDimensions().y,
										   .type		  = Renderer::E_TextureType::TEXTURE_2D,
										   .format		  = Renderer::GetClosestFormat(tex->GetChannels(), !Renderer::IsIntegral(tex->GetStorageType())),
										   .m_bStreamable = true,
										   .m_Levels	  = 10};
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

	GLE_TODO("02-12-2024", "RohacekD", "Use new API for texture rendering?");
	m_TextureList = std::make_unique<GUI::C_LambdaPart>([&]() {
		for (auto& [name, texture] : m_Textures)
		{
			bool selected = false;
			::ImGui::Selectable(name.c_str(), &selected);
			ImGui::Image((void*)(intptr_t)(texture->GetTexture()), ImVec2(128, 128));
			if (selected)
			{
				ReloadTexture(name, texture);
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
C_Texture* C_TextureManager::GetIdentityTexture()
{
	GLE_ASSERT(m_IdentityTexture, "This texture should be preloaded on engine start");
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	auto& glRM	   = dynamic_cast<C_OGLRenderer&>(renderer).GetRMGR();
	return glRM.GetTexture(m_IdentityTexture);
}

//=================================================================================
Renderer::Handle<Renderer::Texture> C_TextureManager::GetIdentityTextureHandle()
{
	return m_IdentityTexture;
}

//=================================================================================
Renderer::I_Device& C_TextureManager::GetDevice()
{
	return m_Device;
}

} // namespace GLEngine::GLRenderer::Textures