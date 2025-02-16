#pragma once

#include <Renderer/RendererApi.h>

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceLoader.h>

namespace GLEngine::Renderer {

class RENDERER_API_EXPORT AnimationResource : public Core::Resource {
public:
	DEFINE_RESOURCE_TYPE(AnimationResource)

	[[nodiscard]] bool Load(const std::filesystem::path& filepath) override;
	[[nodiscard]] bool Reload() override;
	[[nodiscard]] std::unique_ptr<Core::I_ResourceLoader> GetLoader() override;
};

class RENDERER_API_EXPORT AnimationLoader : public Core::ResourceLoader<AnimationResource> {
public:
	std::shared_ptr<Core::Resource> CreateResource() const override;
	std::vector<std::string>		GetSupportedExtensions() const override;
};
} // namespace GLEngine::Renderer
