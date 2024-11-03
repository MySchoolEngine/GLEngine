#pragma once

#include <Renderer/RendererApi.h>

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceLoader.h>

#include <Utils/Serialization/XMLDeserialize.h>

#include <Utils/Reflection/Metadata.h>

namespace GLEngine::Renderer {
class I_TextureViewStorage;

class RENDERER_API_EXPORT TextureResource : public Core::Resource {
public:
	DEFINE_RESOURCE_TYPE(TextureResource)
	TextureResource();
	~TextureResource();

	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Reload() override;

	// First check GetState to avoid SEGFAULTs
	const I_TextureViewStorage& GetStorage() const;
	I_TextureViewStorage&		GetStorage();

private:
	std::unique_ptr<I_TextureViewStorage> m_TextureStorage;

	RTTR_REGISTRATION_FRIEND;
};

class RENDERER_API_EXPORT TextureLoader : public Core::ResourceLoader<TextureResource> {
public:
	virtual std::shared_ptr<Core::Resource> CreateResource() const override;
	virtual std::vector<std::string>		GetSupportedExtensions() const override;
};
} // namespace GLEngine::Renderer

#include <Core/Resources/ResourceHandle.h>
namespace Utils::Reflection::UI {
template <> struct UIMetaclassToType<MetaGUI::Texture> {
	using type = GLEngine::Core::ResourceHandle<GLEngine::Renderer::TextureResource>;
};

enum class Texture
{
	Name,
};
} // namespace UI

namespace Utils::Reflection
{
REGISTER_META_CLASS(UI::Texture, MetaGUI);
REGISTER_META_MEMBER_TYPE(UI::Texture::Name, std::string);
}
