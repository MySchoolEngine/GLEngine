#pragma once

#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/RayCasting/Geometry/BVH.h>
#include <Renderer/RayCasting/Geometry/Trimesh.h>
#include <Renderer/RendererApi.h>

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Renderer {
class RENDERER_API_EXPORT C_TrimeshModel : public Core::Resource {
public:
	DEFINE_RESOURCE_TYPE(MeshResource)
	using T_BaseResource = MeshResource;
	C_TrimeshModel()	 = default;

	[[nodiscard]] static constexpr bool					  IsDerived() { return true; }
	[[nodiscard]] bool									  Load(const std::filesystem::path& filepath) override;
	[[nodiscard]] bool									  Reload() override;
	[[nodiscard]] std::unique_ptr<Core::I_ResourceLoader> GetLoader() override;

	[[nodiscard]] bool Build(Core::ResourceHandle<MeshResource> handle);

	bool SupportSaving() const override;

	void AfterDeserialize();

	RTTR_REGISTRATION_FRIEND;

protected:
	bool SaveInternal() const override;

private:
	std::vector<C_Trimesh> m_Trimeshes;
	std::vector<BVH*>	   m_BVHs;
	std::filesystem::path  m_Filepath;
};

class RENDERER_API_EXPORT TrimeshModelTrimesh : public Core::ResourceLoader<C_TrimeshModel> {
public:
	std::shared_ptr<Core::Resource> CreateResource() const override;
	std::vector<std::string>		GetSupportedExtensions() const override;
};
} // namespace GLEngine::Renderer