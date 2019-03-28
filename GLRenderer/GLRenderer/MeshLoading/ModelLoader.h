#pragma once

#include <string>
#include <vector>
#include <stack>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GLRenderer/MeshLoading/Scene.h>

#define VERTICES_PER_TRIANGLE 3

namespace GLEngine {
namespace GLRenderer {
namespace Mesh {
class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();

	bool addModelFromFileToScene(const char* path, std::shared_ptr<Scene> scene, std::vector< std::string >& textureNames, glm::mat4 sceneTransform = glm::mat4(1));

	void Reset();

private:
	Assimp::Importer*			  _importer;

	std::stack<const aiNode*>     _nodeStack;
	std::stack<glm::mat4>         _transformStack;

	const aiScene* _tryOpenFile(const char* path);

	void        _loadMaterialsFromAiscene(const aiScene* loadedScene, std::shared_ptr<Scene> scene, std::vector< std::string >& textureRegister);
	void        _getMaterialColorAttributes(const aiMaterial* material, Material& mat);
	glm::vec4   _getMaterialColorComponent(const aiMaterial* const material, const char* key, unsigned int type, unsigned int index);
	float       _getMaterialFloatComponent(const aiMaterial* const material, const char* key, unsigned type, unsigned index);
	std::string _getMaterialDiffuseTextureName(const aiMaterial* material);
	int         _getTextureIndexAndAddToRegister(const std::string& name, std::vector< std::string >& textureNames);

	void        _loadMeshesFromAiScene(const aiScene* loadedScene, std::shared_ptr<Scene> scene, const glm::mat4& sceneTransform);
	void        _pushNodeChildrenOnStacks(const aiNode* node, const glm::mat4& nodeWorldTransform);
	void        _getNextNodeAndTransform(const aiNode*& node, glm::mat4& transform);
	void        _loadNodeMeshes(const aiNode* node, const glm::mat4& nodeTransform, aiMesh** const aiMeshes, std::vector<Mesh>& meshes);
	void        _loadSingleMeshFromAimesh(const aiMesh* aiMesh, Mesh& mesh);
	void        _allocateNewMeshes(const unsigned int numNewMeshes, std::vector<Mesh>& meshes);
	void        _getFacePosNormalTcoords(const aiFace* face, const aiMesh* mesh, glm::vec4* pos, glm::vec3* normal, glm::vec2* tcoords);
	void        _allocateMesh(Mesh& mesh, const unsigned int numFaces);
	void        _assignMeshMaterial(Mesh& mesh, const aiMesh* aimesh);

	glm::mat4   _aiMatrixToGlm(const aiMatrix4x4& matrix);

	static unsigned int _numTexturesPreviouslyLoaded;
	static unsigned int _numMaterialsPreviouslyLoaded;
	static unsigned int _numMeshesPreviouslyLoaded;
};
}}}