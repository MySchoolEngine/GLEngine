#include <RendererStdafx.h>

#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/Materials/MeshMaterialExtractor.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Mesh/Scene.h>

#include <Core/Resources/ResourceManager.h>

namespace GLEngine::Renderer {

//=================================================================================
std::vector<Core::ResourceHandle<MaterialResource>> ExtractMaterialsFromMesh(const MeshResource& mesh, bool forceRebuild)
{
	std::vector<Core::ResourceHandle<MaterialResource>> result;

	if (!mesh.IsReady())
		return result;

	const auto& scene = mesh.GetScene();
	auto&		rm	  = Core::C_ResourceManager::Instance();

	for (std::size_t i = 0; i < scene.materials.size(); ++i)
	{
		const auto& mat = scene.materials[i];

		const auto outputPath = MaterialResource::GetOutputPath(mesh, i);

		Core::ResourceHandle<MaterialResource> matHandle;
		// Create a MaterialResource, populate it, and save to disk
		auto newMat = rm.CreateNewResource<MaterialResource>(outputPath);

		if (newMat.has_value() == false)
		{
			if (!forceRebuild)
				continue; // do not force rewrite if not requested

			// if it wasn't loaded because the resource already exists rewrite it
			if (newMat.error() == Core::CreateError::AlreadyExists || newMat.error() == Core::CreateError::AlreadyTracked)
			{
				matHandle = rm.LoadResource<MaterialResource>(outputPath, true);
			}
			else
			{
				CORE_LOG(E_Level::Error, E_Context::Render, "Could not create resource {} due to {}", outputPath, newMat.error());
			}
		}
		else
		{
			matHandle = newMat.value();
		}

		if (matHandle.IsReady() == false)
		{
			continue;
		}

		auto& matRes = matHandle.GetResource();
		matRes.SetMaterialName(mat.m_Name);
		matRes.SetMaterialData(MaterialResource::BuildPBRData(mat, scene.textures));
		if (!matRes.Save())
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Material resource cannot be saved {}", matRes.GetFilePath());
			continue;
		}

		// Load back through ResourceManager so it is properly tracked and deduplicated
		result.push_back(matHandle);
	}

	return result;
}

} // namespace GLEngine::Renderer
