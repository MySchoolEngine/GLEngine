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

	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Reload() override;

	// First check GetState to avoid SEGFAULTs
	MeshData::Scene&	   GetScene();
	const MeshData::Scene& GetScene() const;

	const std::vector<std::filesystem::path>& GetTextureNames() const; // shoudl be handeled better

private:
	std::shared_ptr<Renderer::MeshData::Scene> m_Scene;
	std::vector<std::filesystem::path>		   m_TexuterNames;

	RTTR_REGISTRATION_FRIEND;
};

class RENDERER_API_EXPORT MeshLoader : public Core::ResourceLoader<MeshResource> {
public:
	virtual std::shared_ptr<Core::Resource> CreateResource() const override;
	virtual std::vector<std::string>		GetSupportedExtensions() const override;
};
} // namespace GLEngine::Renderer
