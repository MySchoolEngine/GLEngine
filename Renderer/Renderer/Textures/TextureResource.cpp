#include <RendererStdafx.h>

#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureResource.h>

namespace GLEngine::Renderer {

//=================================================================================
bool TextureResource::Load(const std::filesystem::path& filepath)
{
	Textures::TextureLoader tl;
	m_TextureStorage = tl.loadTexture(filepath);
	return m_TextureStorage != nullptr;
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