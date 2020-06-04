#pragma once

#include <Renderer/Mesh/Scene.h>

#include <assimp/postprocess.h>

#define VERTICES_PER_TRIANGLE 3

struct aiNode;
struct aiMaterial;
struct aiScene;
struct aiMesh;
struct aiFace;
namespace Assimp
{
class Importer;
}

namespace GLEngine::GLRenderer::Mesh {
class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();

	[[nodiscard]] bool addModelFromFileToScene(const char* path, std::shared_ptr<Renderer::MeshData::Scene> scene, std::vector<std::string>& textureNames, glm::mat4 sceneTransform = glm::mat4(1));

	void Reset();

private:
	std::unique_ptr<Assimp::Importer>	_importer;

	std::stack<const aiNode*>					_nodeStack;
	std::stack<glm::mat4>							_transformStack;

	const aiScene* _tryOpenFile(const char* path);

	void        _loadMaterialsFromAiscene(const aiScene* loadedScene, std::shared_ptr<Renderer::MeshData::Scene> scene, std::vector< std::string >& textureRegister);
	void        _getMaterialColorAttributes(const aiMaterial* material, Renderer::MeshData::Material& mat);
	glm::vec4   _getMaterialColorComponent(const aiMaterial* const material, const char* key, unsigned int type, unsigned int index);
	float       _getMaterialFloatComponent(const aiMaterial* const material, const char* key, unsigned type, unsigned index);
	std::string _getMaterialDiffuseTextureName(const aiMaterial* material);
	int         _getTextureIndexAndAddToRegister(const std::string& name, std::vector< std::string >& textureNames);

	void        _loadMeshesFromAiScene(const aiScene* loadedScene, std::shared_ptr<Renderer::MeshData::Scene> scene, const glm::mat4& sceneTransform);
	void        _pushNodeChildrenOnStacks(const aiNode* node, const glm::mat4& nodeWorldTransform);
	void        _getNextNodeAndTransform(const aiNode*& node, glm::mat4& transform);
	void        _loadNodeMeshes(const aiNode* node, const glm::mat4& nodeTransform, aiMesh** const aiMeshes, std::vector<Renderer::MeshData::Mesh>& meshes);
	void        _loadSingleMeshFromAimesh(const aiMesh* aiMesh, Renderer::MeshData::Mesh& mesh);
	void        _allocateNewMeshes(const unsigned int numNewMeshes, std::vector<Renderer::MeshData::Mesh>& meshes);
	void        _getFacePosNormalTcoords(const aiFace* face, const aiMesh* mesh, glm::vec4* pos, glm::vec3* normal, glm::vec2* tcoords, glm::vec3* ttangents, glm::vec3* tbitangents);
	void        _allocateMesh(Renderer::MeshData::Mesh& mesh, const unsigned int numFaces);
	void        _assignMeshMaterial(Renderer::MeshData::Mesh& mesh, const aiMesh* aimesh);

	glm::mat4   _aiMatrixToGlm(const aiMatrix4x4& matrix);

	static unsigned int _numTexturesPreviouslyLoaded;
	static unsigned int _numMaterialsPreviouslyLoaded;
	static unsigned int _numMeshesPreviouslyLoaded;
};
}