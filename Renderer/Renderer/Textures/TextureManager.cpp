#include <RendererStdafx.h>

#include <Renderer/Textures/TextureManager.h>

namespace GLEngine::Renderer {

//=================================================================================
C_TextureManager& C_TextureManager::CreateInstance(I_Device& device)
{
	static C_TextureManager instance(device); // Guaranteed to be destroyed.
											  // Instantiated on first use.
	return instance;
}

//=================================================================================
I_DeviceTexture* C_TextureManager::GetTexture(const Core::ResourceHandle<Renderer::TextureResource>& resource)
{
	// auto it = m_Textures.find(resource);
	// if (it != m_Textures.end())
	// {
	// 	return it->second;
	// }
	return nullptr;
}

//=================================================================================
C_TextureManager::C_TextureManager(I_Device& device)
	: m_Device(device)
{
}

} // namespace GLEngine::Renderer
