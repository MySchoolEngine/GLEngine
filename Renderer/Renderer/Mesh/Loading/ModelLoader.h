#pragma once

#include <Renderer/Mesh/Scene.h>

#define VERTICES_PER_TRIANGLE 3

struct aiNode;
struct aiMaterial;
struct aiScene;
struct aiMesh;
struct aiFace;

namespace Assimp {
class Importer;
}

namespace GLEngine::Renderer::Mesh {
class ModelLoader {
public:
	ModelLoader();
	~ModelLoader();

	[[nodiscard]] bool addModelFromFileToScene(const std::filesystem::path&		   path,
											   std::shared_ptr<MeshData::Scene>	   scene,
											   std::vector<std::filesystem::path>& textureNames,
											   glm::mat4						   sceneTransform = glm::mat4(1));

	void		Reset();
	std::mutex& GetMutex();


private:
	static std::mutex				  m_Mutex;
	std::unique_ptr<Assimp::Importer> _importer;

	std::stack<const aiNode*> _nodeStack;
	std::stack<glm::mat4>	  _transformStack;

	const aiScene* _tryOpenFile(const std::filesystem::path& path);

	void				  _loadMaterialsFromAiscene(const aiScene* loadedScene, std::shared_ptr<MeshData::Scene> scene, std::vector<std::filesystem::path>& textureRegister);
	void				  _loadLightsFromAiScene(const aiScene* loadedScene, std::shared_ptr<MeshData::Scene> scene);
	void				  _loadSkeletonFromAiScene(const aiMesh* aiMesh, MeshData::Mesh& mesh);
	void				  _getMaterialColorAttributes(const aiMaterial* material, MeshData::Material& mat);
	Colours::T_Colour	  _getMaterialColorComponent(const aiMaterial* const material, const char* key, unsigned int type, unsigned int index);
	float				  _getMaterialFloatComponent(const aiMaterial* const material, const char* key, unsigned type, unsigned index);
	std::filesystem::path _getMaterialDiffuseTextureName(const aiMaterial* material);
	std::string			  _getMaterialNormalTextureName(const aiMaterial* material);
	int					  _getTextureIndexAndAddToRegister(const std::filesystem::path& name, std::vector<std::filesystem::path>& textureNames);

	void _loadMeshesFromAiScene(const aiScene* loadedScene, std::shared_ptr<MeshData::Scene> scene, const glm::mat4& sceneTransform);
	void _pushNodeChildrenOnStacks(const aiNode* node, const glm::mat4& nodeWorldTransform);
	void _getNextNodeAndTransform(const aiNode*& node, glm::mat4& transform);
	void _loadNodeMeshes(const aiNode* node, const glm::mat4& nodeTransform, aiMesh** const aiMeshes, std::vector<MeshData::Mesh>& meshes);
	void _loadSingleMeshFromAimesh(const aiMesh* aiMesh, MeshData::Mesh& mesh);
	void _allocateNewMeshes(const unsigned int numNewMeshes, std::vector<MeshData::Mesh>& meshes);
	void _getFacePosNormalTcoords(const aiFace* face, const aiMesh* mesh, glm::vec4* pos, glm::vec3* normal, glm::vec2* tcoords, glm::vec3* ttangents, glm::vec3* tbitangents);
	void _allocateMesh(MeshData::Mesh& mesh, const unsigned int numFaces);
	void _assignMeshMaterial(MeshData::Mesh& mesh, const aiMesh* aimesh);
	void _loadArmatureData(const aiNode* currentNode, const glm::mat4& currentTransform, MeshData::Mesh& mesh);

	static unsigned int _numTexturesPreviouslyLoaded;
	static unsigned int _numMaterialsPreviouslyLoaded;
	static unsigned int _numMeshesPreviouslyLoaded;
};
} // namespace GLEngine::Renderer::Mesh