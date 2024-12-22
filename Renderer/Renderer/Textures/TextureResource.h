#pragma once

#include <Renderer/RendererApi.h>

#include <Core/Filesystem/Paths.h>
#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceLoader.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/XMLDeserialize.h>

namespace GLEngine::Renderer {
class I_TextureViewStorage;

class RENDERER_API_EXPORT TextureResource : public Core::Resource {
public:
	DEFINE_RESOURCE_WITH_PATH_TYPE(TextureResource, Core::Filesystem::imageFolder)
	TextureResource();
	~TextureResource() override;

	[[nodiscard]] bool									  Load(const std::filesystem::path& filepath) override;
	[[nodiscard]] bool									  Reload() override;
	[[nodiscard]] std::unique_ptr<Core::I_ResourceLoader> GetLoader() override;

	// First check GetState to avoid SEGFAULTs
	const I_TextureViewStorage& GetStorage() const;
	I_TextureViewStorage&		GetStorage();

private:
	std::unique_ptr<I_TextureViewStorage> m_TextureStorage;

	RTTR_REGISTRATION_FRIEND;
};

class RENDERER_API_EXPORT TextureLoader : public Core::ResourceLoader<TextureResource> {
public:
	std::shared_ptr<Core::Resource> CreateResource() const override;
	std::vector<std::string>		GetSupportedExtensions() const override;
};
} // namespace GLEngine::Renderer

#include <Core/Resources/ResourceHandle.h>

namespace Utils::Reflection::UI {
template <> struct UIMetaclassToType<MetaGUI::Texture> {
	using type = GLEngine::Core::ResourceHandle<GLEngine::Renderer::TextureResource>;
};

enum class Texture : std::uint8_t {
	Name,
};
} // namespace Utils::Reflection::UI

namespace Utils::Reflection {
REGISTER_META_CLASS(UI::Texture, MetaGUI);

REGISTER_META_MEMBER_TYPE(UI::Texture::Name, std::string);
} // namespace Utils::Reflection