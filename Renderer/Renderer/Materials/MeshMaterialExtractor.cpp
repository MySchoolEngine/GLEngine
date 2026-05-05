#include <RendererStdafx.h>

#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/Materials/MeshMaterialExtractor.h>
#include <Renderer/Materials/PBRMaterialData.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Textures/TextureResource.h>

#include <Core/Resources/ResourceManager.h>

namespace GLEngine::Renderer {

//=================================================================================
std::vector<Core::ResourceHandle<MaterialResource>> ExtractMaterialsFromMesh(const MeshResource& mesh, const std::filesystem::path& outputDir)
{
	std::vector<Core::ResourceHandle<MaterialResource>> result;

	if (!mesh.IsReady())
		return result;

	const auto& scene	 = mesh.GetScene();
	auto&		rm		 = Core::C_ResourceManager::Instance();
	const auto	meshStem = mesh.GetFilePath().stem().string();

	for (std::size_t i = 0; i < scene.materials.size(); ++i)
	{
		const auto& mat = scene.materials[i];

		// Prefer material name for the filename, fall back to index
		std::string matName = mat.m_Name.empty() ? ("material" + std::to_string(i)) : mat.m_Name;

		// Sanitize for filesystem: replace spaces and path separators
		for (auto& c : matName)
		{
			if (c == ' ' || c == '/' || c == '\\')
				c = '_';
		}

		const auto outputPath = outputDir / (meshStem + "-" + matName + ".glmat");

		// Build PBR data from MeshData::Material
		auto data = std::make_shared<C_PBRMaterialData>(mat.m_Name);
		data->SetColor(mat.diffuse);

		// shininess → roughness: invert and normalize (shininess 0 = rough, 128+ = smooth)
		const float roughness = 1.f - std::clamp(mat.shininess / 128.f, 0.f, 1.f);
		data->SetRoughness(roughness);

		if (mat.textureIndex >= 0 && static_cast<std::size_t>(mat.textureIndex) < scene.textures.size())
		{
			const auto& texPath = scene.textures[static_cast<std::size_t>(mat.textureIndex)];
			if (!texPath.empty())
				data->SetColorMapRes(rm.LoadResource<TextureResource>(texPath));
		}

		if (mat.normalTextureIndex >= 0 && static_cast<std::size_t>(mat.normalTextureIndex) < scene.textures.size())
		{
			const auto& normalPath = scene.textures[static_cast<std::size_t>(mat.normalTextureIndex)];
			if (!normalPath.empty())
				data->SetNormalMapRes(rm.LoadResource<TextureResource>(normalPath));
		}

		// Create a MaterialResource, populate it, and save to disk
		auto matRes = std::make_shared<MaterialResource>();
		matRes->SetMaterialName(mat.m_Name);
		matRes->SetMaterialData(std::move(data));
		matRes->InitAndSave(outputPath);

		// Load back through ResourceManager so it is properly tracked and deduplicated
		result.push_back(rm.LoadResource<MaterialResource>(outputPath, /*isBlocking=*/true));
	}

	return result;
}

} // namespace GLEngine::Renderer
