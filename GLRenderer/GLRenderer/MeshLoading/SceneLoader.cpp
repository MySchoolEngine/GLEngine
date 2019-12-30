#include <GLRendererStdafx.h>

#include <GLRenderer/MeshLoading/SceneLoader.h>

#include <GLRenderer/Textures/TextureLoader.h>
#include <GLRenderer/MeshLoading/ModelLoader.h>

#include <REnderer/Mesh/Scene.h>

namespace GLEngine::GLRenderer::Mesh {

//=================================================================================
bool SceneLoader::addModelFromFileToScene(const char* filepath, const char* filename, 
											std::shared_ptr<Renderer::MeshData::Scene> scene, const glm::mat4& transform)
{
	//Load geometry first
	ModelLoader ml;
	// dr
	ml.Reset();
	std::vector< std::string > texNames;

	bool retval = ml.addModelFromFileToScene((std::string(filepath) + std::string("/") + std::string(filename)).c_str(), scene, texNames, transform);

	if (!retval)
		return false;

	for (const auto& mesh : scene->meshes)
		scene->bbox.Add(mesh.bbox.getTransformedAABB(mesh.modelMatrix));

	Textures::TextureLoader tl;

	for (const auto & texName : texNames)
	{
		Renderer::MeshData::Texture t;
		retval = tl.loadTexture((std::string(filepath) + std::string("\\") + texName).c_str(), t);

		if (!retval)
			return false;

		scene->textures.push_back(t);
	}

	return true;
}

}