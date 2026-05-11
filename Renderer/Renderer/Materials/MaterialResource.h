#pragma once

#include <Renderer/Materials/I_MaterialData.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/RendererApi.h>

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>

#include <memory>
#include <rttr/registration_friend.h>

namespace GLEngine::Renderer {

class RENDERER_API_EXPORT MaterialResource : public Core::Resource {
public:
	DEFINE_RESOURCE_TYPE(MaterialResource)
	using T_BaseResource = MeshResource;

	MaterialResource() = default;

	// Build() returns false for meshes with >1 materials — use MeshMaterialExtractor instead.
	[[nodiscard]] static constexpr bool IsDerived() { return true; }

	[[nodiscard]] bool									  Load(const std::filesystem::path& filepath) override;
	[[nodiscard]] bool									  Reload() override;
	[[nodiscard]] std::unique_ptr<Core::I_ResourceLoader> GetLoader() override;

	// Build from a single-material mesh. Returns false (and does nothing) for multi-material meshes.
	[[nodiscard]] bool Build(const MeshResource& mesh);

	// Builds PBR material data from a single MeshData::Material entry and its texture list.
	// Used by Build() and MeshMaterialExtractor to avoid duplication.
	[[nodiscard]] static std::shared_ptr<I_MaterialData> BuildPBRData(
		const MeshData::Material&					  mat,
		const std::vector<std::filesystem::path>& textures);

	bool SupportSaving() const override { return true; }

	const std::filesystem::path& GetShaderPath() const { return m_ShaderPath; }
	void						 SetShaderPath(const std::filesystem::path& path) { m_ShaderPath = path; }

	const std::string& GetMaterialName() const { return m_MaterialName; }
	void			   SetMaterialName(const std::string& name) { m_MaterialName = name; }

	I_MaterialData*		  GetMaterialData() { return m_Material.get(); }
	const I_MaterialData* GetMaterialData() const { return m_Material.get(); }
	void				  SetMaterialData(std::shared_ptr<I_MaterialData> data) { m_Material = std::move(data); }

	// Used by MeshMaterialExtractor to set the filepath and trigger initial save
	void InitAndSave(const std::filesystem::path& path)
	{
		m_Filepath = path;
		m_Dirty	   = true;
		Save();
	}

	bool DrawGUI();

	RTTR_REGISTRATION_FRIEND;

protected:
	bool SaveInternal() const override;

private:
	std::string						m_MaterialName;
	std::filesystem::path			m_ShaderPath;
	std::shared_ptr<I_MaterialData> m_Material;
};

class RENDERER_API_EXPORT MaterialResourceLoader : public Core::ResourceLoader<MaterialResource> {
public:
	std::shared_ptr<Core::Resource> CreateResource() const override;
	std::vector<std::string>		GetSupportedExtensions() const override;
};

static_assert(Core::IsBeDerivedResource<MaterialResource>, "MaterialResource must satisfy IsBeDerivedResource");
static_assert(Core::IsBuildableResource<MaterialResource, MaterialResource::T_BaseResource>, "MaterialResource must satisfy IsBuildableResource");

} // namespace GLEngine::Renderer
