#pragma once

#include <Renderer/RendererApi.h>

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceLoader.h>

namespace GLEngine::Renderer {
class I_TextureViewStorage;

class RENDERER_API_EXPORT TextureResource : public Core::Resource {
public:
	DECLARE_RESOURCE_TYPE(TextureResource)
	TextureResource();
	~TextureResource();

	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Reload() override;

	// First check GetState to avoid SEGFAULTs
	const I_TextureViewStorage&	 GetStorage() const;
	I_TextureViewStorage&		 GetStorage();
	const std::filesystem::path& GetFilepath() const { return m_Filepath; }

private:
	std::filesystem::path				  m_Filepath;
	std::unique_ptr<I_TextureViewStorage> m_TextureStorage;
};

class RENDERER_API_EXPORT TextureLoader : public Core::ResourceLoader<TextureResource> {
public:
	virtual std::shared_ptr<Core::Resource> CreateResource() const override;
	virtual std::vector<std::string>		GetSupportedExtensions() const override;
};
} // namespace GLEngine::Renderer
