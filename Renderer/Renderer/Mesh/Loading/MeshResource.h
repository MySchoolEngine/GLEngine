#pragma once

#include <Renderer/RendererApi.h>

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceLoader.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Renderer {

namespace MeshData {
struct Scene;
}

class RENDERER_API_EXPORT MeshResource : public Core::Resource {
public:
	DEFINE_RESOURCE_TYPE(MeshResource)

	MeshResource();

	[[nodiscard]] bool							  Load(const std::filesystem::path& filepath) override;
	[[nodiscard]] bool							  Reload() override;
	[[nodiscard]] std::unique_ptr<Core::I_ResourceLoader> GetLoader() override;

	// First check GetState to avoid SEGFAULTs
	MeshData::Scene&	   GetScene();
	const MeshData::Scene& GetScene() const;

	const std::vector<std::filesystem::path>& GetTextureNames() const; // should be handled better

private:
	std::shared_ptr<MeshData::Scene>   m_Scene;
	std::vector<std::filesystem::path> m_TextureNames;

	RTTR_REGISTRATION_FRIEND;
};

class RENDERER_API_EXPORT MeshLoader : public Core::ResourceLoader<MeshResource> {
public:
	virtual std::shared_ptr<Core::Resource> CreateResource() const override;
	virtual std::vector<std::string>		GetSupportedExtensions() const override;
};
} // namespace GLEngine::Renderer
