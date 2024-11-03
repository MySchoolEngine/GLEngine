#include <RendererStdafx.h>

#include <Renderer/IDevice.h>
#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureManager.h>

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
I_DeviceTexture* C_TextureManager::GetOrCreateTexture(const Core::ResourceHandle<Renderer::TextureResource> resource)
{
	//auto it = m_Textures.find(resource);
	//if (it != m_Textures.end())
	//{
	//	return it->second.get();
	//}
	//m_Textures[resource] = m_ErrorTexture;

	// if (resource.IsReady())
	// 	m_Textures[resource] = CreateTexture(resource);

	m_ToLoad.push_back(resource);

	return nullptr;
}

void C_TextureManager::Update()
{
}

//=================================================================================
C_TextureManager::C_TextureManager(I_Device& device)
	: m_Device(device)
{
	// identity texture
	{
		const Renderer::TextureDescriptor desc{.name		  = "Identity texture",
											   .width		  = 1,
											   .height		  = 1,
											   .type		  = Renderer::E_TextureType::TEXTURE_2D,
											   .format		  = Renderer::E_TextureFormat::RGBA16f,
											   .m_bStreamable = false};

		m_IdentityTexture = m_Device.CreateTextureHandle(desc);
	}
	// error texture
	{
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
											   .m_bStreamable = false};

		m_ErrorTexture = m_Device.CreateTextureHandle(desc);
	}
}

std::shared_ptr<I_DeviceTexture> C_TextureManager::CreateTexture(Core::ResourceHandle<Renderer::TextureResource> resource)
{
	GLE_ASSERT(resource.IsReady(), "Resource not ready yet.");
	I_TextureViewStorage& storage  = resource.GetResource().GetStorage();
	auto				  texture = m_Device.CreateTextureHandle(
		Renderer::TextureDescriptor{
			.name		  = resource.GetFilePath().generic_string(),
			.width		  = storage.GetDimensions().x,
			.height		  = storage.GetDimensions().y,
			.type		  = Renderer::E_TextureType::TEXTURE_2D,
			.format		  = Renderer::GetClosestFormat(storage.GetChannels(),!Renderer::IsIntegral(storage.GetStorageType())),
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
