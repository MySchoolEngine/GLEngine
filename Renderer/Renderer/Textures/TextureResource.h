#pragma once

#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureStorage.h>

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceManager.h>

namespace GLEngine::Renderer {

class TextureResource : public Core::Resource {
public:
	DECLARE_RESOURCE_TYPE(TextureResource)

	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Reload() override { return true; }

private:
	std::filesystem::path				  m_Filepath;
	std::unique_ptr<I_TextureViewStorage> m_TextureStorage = nullptr;
};

class RENDERER_API_EXPORT TextureLoader : public Core::ResourceLoader<TextureResource> {
public:
	virtual std::shared_ptr<Core::Resource> CreateResource() const override;
	virtual std::vector<std::string>		GetSupportedExtensions() const override;
};
} // namespace GLEngine::Renderer
