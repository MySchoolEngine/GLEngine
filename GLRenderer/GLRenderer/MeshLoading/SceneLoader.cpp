#include <GLRendererStdafx.h>

#include <GLRenderer/MeshLoading/SceneLoader.h>
#include <GLRenderer/MeshLoading/ModelLoader.h>

namespace GLEngine {
namespace GLRenderer {
namespace Mesh {

//=================================================================================
bool SceneLoader::addModelFromFileToScene(const char* filepath, const char* filename, 
											std::shared_ptr<Scene> scene, const glm::mat4& transform)
{
	//Load geometry first
	ModelLoader ml;
	// dr
	ml.Reset();
	std::vector< std::string > texNames;

	bool retval = ml.addModelFromFileToScene((std::string(filepath) + std::string("/") + std::string(filename)).c_str(), scene, texNames, transform);

	if (!retval)
		return false;

	for (auto mesh : scene->meshes)
		scene->bbox.Add(mesh.bbox.getTransformedAABB(mesh.modelMatrix));

	/*TextureLoader tl;

	for (unsigned int i = 0; i < texNames.size(); ++i)
	{
		Texture t;
		retval = tl.loadTexture((std::string(filepath) + std::string("\\") + texNames[i]).c_str(), t);

		if (!retval)
			return false;

		scene->textures.push_back(t);
	}*/

	return true;
}
}}}