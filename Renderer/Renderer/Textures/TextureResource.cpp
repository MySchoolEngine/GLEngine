#include <RendererStdafx.h>

#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureResource.h>
#include <Renderer/Textures/TextureStorage.h>

namespace GLEngine::Renderer {

//=================================================================================
bool TextureResource::Load(const std::filesystem::path& filepath)
{
	Textures::TextureLoader tl;

	std::lock_guard lock(tl.GetMutex());
	m_TextureStorage = tl.loadTexture(filepath);
	m_Filepath		 = filepath;
	return m_TextureStorage != nullptr;
}

//=================================================================================
bool TextureResource::Reload()
{
	Textures::TextureLoader tl;
	auto					newStorage = tl.loadTexture(m_Filepath);
	if (!newStorage)
	{
		CORE_LOG(E_Level::Warning, E_Context::Render, "Unable to reload image {}", m_Filepath);
		return false;
	}
	m_TextureStorage = std::move(newStorage);
	return true;
}

//=================================================================================
const I_TextureViewStorage& TextureResource::GetStorage() const
{
	return *m_TextureStorage.get();
}

//=================================================================================
I_TextureViewStorage& TextureResource::GetStorage()
{
	return *m_TextureStorage.get();
}

//=================================================================================
std::shared_ptr<Core::Resource> TextureLoader::CreateResource() const
{
	return std::make_shared<TextureResource>();
}

//=================================================================================
std::vector<std::string> TextureLoader::GetSupportedExtensions() const
{
	return {".png", ".jpg", ".bmp"};
}

} // namespace GLEngine::Renderer