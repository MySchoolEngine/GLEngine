#include <RendererStdafx.h>

#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Resources/ResourceManager.h>
#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureManager.h>

#include <Core/Application.h>

namespace GLEngine::Renderer {
std::filesystem::path C_TextureManager::s_ErrorTextureFile = "Models/Error.bmp";

//=================================================================================
C_TextureManager& C_TextureManager::CreateInstance(I_Device& device)
{
	static C_TextureManager instance(device); // Guaranteed to be destroyed.
											  // Instantiated on first use.
	return instance;
}

//=================================================================================
Handle<Texture> C_TextureManager::GetOrCreateTexture(const Core::ResourceHandle<TextureResource> resource)
{
	auto it = m_Textures.find(resource);
	if (it != m_Textures.end())
	{
		return it->second;
	}

	if (resource.IsReady())
	{
		auto&		renderer	= Core::C_Application::Get().GetActiveRenderer();
		const auto& resourcePtr = resource.GetResource();
		const auto& storage		= resourcePtr.GetStorage();

		const auto GPUSamplerHandle = renderer.GetRM().createSampler(SamplerDescriptor2D{
			.m_FilterMin = E_TextureFilter::Linear,
			.m_FilterMag = E_TextureFilter::Linear,
			.m_WrapS	 = E_WrapFunction::Repeat,
			.m_WrapT	 = E_WrapFunction::Repeat,
			.m_WrapU	 = E_WrapFunction::Repeat,
		});

		Handle<Texture> handle = renderer.GetRM().createTexture(TextureDescriptor{.name			 = resource.GetFilePath().generic_string(),
																				  .width		 = storage.GetDimensions().x,
																				  .height		 = storage.GetDimensions().y,
																				  .type			 = E_TextureType::TEXTURE_2D,
																				  .format		 = GetClosestFormat(storage.GetChannels(), !IsIntegral(storage.GetStorageType())),
																				  .m_bStreamable = false});
		renderer.SetTextureSampler(handle, GPUSamplerHandle);
		// upload texture
		renderer.SetTextureData(handle, storage);
		m_Textures.emplace(resource, handle);
		return handle;
	}
	else
	{
		m_ToLoad.push_back(resource);
	}

	// m_Textures[resource] = m_ErrorTexture;

	// if (resource.IsReady())
	// 	m_Textures[resource] = CreateTexture(resource);

	// we need to check the resource have been loaded into the memory, and then load it onto the GPU


	return {};
}

//=================================================================================
Handle<Texture> C_TextureManager::GetTexture(const Core::ResourceHandle<TextureResource> resource) const
{
	if (!resource)
		return {};
	auto it = m_Textures.find(resource);
	if (it != m_Textures.end())
	{
		return it->second;
	}
	return {};
}

//=================================================================================
void C_TextureManager::Update()
{
}

//=================================================================================
C_TextureManager::C_TextureManager(I_Device& device)
	: m_Device(device)
{
	// identity texture
	{
		const TextureDescriptor desc{.name			= "Identity texture",
									 .width			= 1,
									 .height		= 1,
									 .type			= E_TextureType::TEXTURE_2D,
									 .format		= E_TextureFormat::RGBA16f,
									 .m_bStreamable = false};

		m_IdentityTexture = m_Device.CreateTextureHandle(desc);
	}
	// error texture
	{
		Textures::TextureLoader tl;
		const auto				buffer = tl.loadTexture(s_ErrorTextureFile.generic_string().c_str());
		if (!buffer)
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Could not load texture '{}'", s_ErrorTextureFile.generic_string());
			return;
		}

		const TextureDescriptor desc{.name			= s_ErrorTextureFile.generic_string(),
									 .width			= buffer->GetDimensions().x,
									 .height		= buffer->GetDimensions().y,
									 .type			= E_TextureType::TEXTURE_2D,
									 .format		= E_TextureFormat::RGBA32f,
									 .m_bStreamable = false};

		m_ErrorTexture = m_Device.CreateTextureHandle(desc);
	}
}

//=================================================================================
std::shared_ptr<I_DeviceTexture> C_TextureManager::CreateTexture(Core::ResourceHandle<TextureResource> resource)
{
	GLE_ASSERT(resource.IsReady(), "Resource not ready yet.");
	const I_TextureViewStorage& storage = resource.GetResource().GetStorage();
	auto						texture = m_Device.CreateTextureHandle(TextureDescriptor{.name	 = resource.GetFilePath().generic_string(),
																						 .width	 = storage.GetDimensions().x,
																						 .height = storage.GetDimensions().y,
																						 .type	 = E_TextureType::TEXTURE_2D,
																						 .format = GetClosestFormat(storage.GetChannels(), !IsIntegral(storage.GetStorageType())),
																						 .m_bStreamable = false});
	if (!m_Device.AllocateTexture(*texture.get()))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Cannot allocate memory for texture '{}'", resource.GetFilePath());
		return nullptr; // TODO
	}
	texture->SetTexData2D(0, (&storage));
	return texture;
}

} // namespace GLEngine::Renderer
