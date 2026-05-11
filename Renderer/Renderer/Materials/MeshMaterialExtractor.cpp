#include <RendererStdafx.h>

#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/Materials/MeshMaterialExtractor.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Mesh/Scene.h>

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

		// Create a MaterialResource, populate it, and save to disk
		auto matRes = std::make_shared<MaterialResource>();
		matRes->SetMaterialName(mat.m_Name);
		matRes->SetMaterialData(MaterialResource::BuildPBRData(mat, scene.textures));
		matRes->InitAndSave(outputPath);

		// Load back through ResourceManager so it is properly tracked and deduplicated
		result.push_back(rm.LoadResource<MaterialResource>(outputPath, /*isBlocking=*/true));
	}

	return result;
}

} // namespace GLEngine::Renderer
