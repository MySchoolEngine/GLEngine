#include <GLRendererStdafx.h>

#include <GLRenderer/MeshLoading/ModelLoader.h>

#include <string>

namespace GLEngine {
namespace GLRenderer {
namespace Mesh {

//=================================================================================
unsigned int ModelLoader::_numTexturesPreviouslyLoaded = 0;
unsigned int ModelLoader::_numMaterialsPreviouslyLoaded = 0;
unsigned int ModelLoader::_numMeshesPreviouslyLoaded = 0;

//=================================================================================
ModelLoader::ModelLoader()
{
	_importer = new Assimp::Importer();
}

//=================================================================================
ModelLoader::~ModelLoader()
{
	if (_importer)
		delete _importer;
}

//=================================================================================
bool ModelLoader::addModelFromFileToScene(const char* path, std::shared_ptr<Scene> scene, std::vector< std::string >& textureRegister, glm::mat4 sceneTransform)
{
    auto loadedScene = _tryOpenFile(path);
    
    if(loadedScene==nullptr)
        return false;

    _loadMaterialsFromAiscene(loadedScene, scene, textureRegister);

	_loadMeshesFromAiScene(loadedScene, scene, sceneTransform);

	_importer->FreeScene();

	_numMaterialsPreviouslyLoaded += loadedScene->mNumMaterials;

    return true;
}

//=================================================================================
void ModelLoader::Reset()
{
	_numMaterialsPreviouslyLoaded = _numMeshesPreviouslyLoaded = _numTexturesPreviouslyLoaded = 0;
}

//=================================================================================
void ModelLoader::_loadMaterialsFromAiscene(const aiScene* loadedScene, std::shared_ptr<Scene> scene, std::vector< std::string >& textureRegister)
{
    for(unsigned int i=0; i<loadedScene->mNumMaterials; ++i)
    {
        Material m;
        _getMaterialColorAttributes(loadedScene->mMaterials[i], m);
        const std::string texName = _getMaterialDiffuseTextureName(loadedScene->mMaterials[i]);
        m.textureIndex = _getTextureIndexAndAddToRegister(texName, textureRegister);
        scene->materials.push_back(m);
    }
}

//=================================================================================
const aiScene* ModelLoader::_tryOpenFile(const char* path)
{
    const aiScene* s = _importer->ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_RemoveRedundantMaterials | aiProcess_GenUVCoords);
    
    if(s == nullptr)
    {
        std::cerr << "Failed to import " << std::string(path) << std::endl;
        return nullptr;
    }

    return s;
}

//=================================================================================
void ModelLoader::_getMaterialColorAttributes(const aiMaterial* const material, Material& mat)
{
    mat.ambient = _getMaterialColorComponent(material, AI_MATKEY_COLOR_AMBIENT);

    mat.diffuse = _getMaterialColorComponent(material, AI_MATKEY_COLOR_DIFFUSE);
    mat.diffuse.w = _getMaterialFloatComponent(material, AI_MATKEY_OPACITY);

    mat.specular = _getMaterialColorComponent(material, AI_MATKEY_COLOR_SPECULAR);

    mat.shininess = _getMaterialFloatComponent(material, AI_MATKEY_SHININESS);
}

//=================================================================================
glm::vec4 ModelLoader::_getMaterialColorComponent(const aiMaterial* const material, const char* key, unsigned type, unsigned index)
{
    if(material)
    {
        aiColor3D c;
        material->Get(key,type, index, c);
        return glm::vec4(c.r, c.g, c.b, 1.0f);
    }
    
    return glm::vec4(0, 0, 0, 0);
}

//=================================================================================
float ModelLoader::_getMaterialFloatComponent(const aiMaterial* material, const char* key, unsigned type, unsigned index)
{
    if(material)
    {
        float f;
        material->Get(key, type, index, f);
        return f;
    }
    
    return 0;
}

//=================================================================================
std::string ModelLoader::_getMaterialDiffuseTextureName(const aiMaterial* material)
{
    if(material && material->GetTextureCount(aiTextureType_DIFFUSE)!=0)
    {
        aiString tpath;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &tpath);
        return tpath.C_Str();
    }

    return "";
}

//=================================================================================
int ModelLoader::_getTextureIndexAndAddToRegister(const std::string& name, std::vector< std::string >& textureNames)
{
    if(name.empty())
        return -1;

    textureNames.push_back(name);
    return _numTexturesPreviouslyLoaded++;
}

//=================================================================================
void ModelLoader::_loadMeshesFromAiScene(const aiScene* loadedScene, std::shared_ptr<Scene> scene, const glm::mat4& sceneTransform)
{
    const aiNode* currentNode = loadedScene->mRootNode;
    glm::mat4 currentTransform = sceneTransform * _aiMatrixToGlm(loadedScene->mRootNode->mTransformation);

    while(currentNode!=nullptr)
    {
        _pushNodeChildrenOnStacks(currentNode, currentTransform);

		_loadNodeMeshes(currentNode, currentTransform, loadedScene->mMeshes, scene->meshes);

        _getNextNodeAndTransform(currentNode, currentTransform);
    }
}

//=================================================================================
void ModelLoader::_pushNodeChildrenOnStacks(const aiNode* node, const glm::mat4& nodeWorldTransform)
{
    const unsigned int numChildren = node->mNumChildren;

    for(unsigned int i=0; i<numChildren; ++i)
    {
        _nodeStack.push(node->mChildren[i]);
        _transformStack.push(nodeWorldTransform * _aiMatrixToGlm(node->mTransformation));
    }
}

//=================================================================================
glm::mat4 ModelLoader::_aiMatrixToGlm(const aiMatrix4x4& aiMatrix)
{
    aiMatrix4x4 temp = aiMatrix;
	temp.Inverse();

    glm::mat4 glmMat;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			glmMat[i][j] = temp[i][j];

    return glmMat;
}

//=================================================================================
void ModelLoader::_getNextNodeAndTransform(const aiNode*& node, glm::mat4& transform)
{
    if(_nodeStack.empty())
    {
        node = nullptr;
        return;
    }

    node = _nodeStack.top();
    _nodeStack.pop();

    transform = _transformStack.top();
    _transformStack.pop();
}

//=================================================================================
void ModelLoader::_loadNodeMeshes(const aiNode* node, const glm::mat4& nodeTransform, aiMesh** const aiMeshes, std::vector<Mesh>& meshes)
{
    _allocateNewMeshes(node->mNumMeshes, meshes);
    
    for(unsigned int i = 0; i<node->mNumMeshes; ++i)
    {
		const unsigned int meshIndex = node->mMeshes[i];
		_loadSingleMeshFromAimesh(aiMeshes[meshIndex], meshes[_numMeshesPreviouslyLoaded + i]);

		meshes[_numMeshesPreviouslyLoaded + i].modelMatrix = nodeTransform;
		meshes[_numMeshesPreviouslyLoaded + i].m_name = std::string(node->mName.C_Str());
    }

	_numMeshesPreviouslyLoaded += node->mNumMeshes;
}

//=================================================================================
void ModelLoader::_loadSingleMeshFromAimesh(const aiMesh* aiMesh, Mesh& mesh)
{
    _allocateMesh(mesh, aiMesh->mNumFaces);
    _assignMeshMaterial(mesh, aiMesh);
        
	for (unsigned int i = 0; i < aiMesh->mNumFaces; ++i)
	{
		aiFace* f = &aiMesh->mFaces[i];
		
		glm::vec4* newPositions = &(mesh.vertices[VERTICES_PER_TRIANGLE * i]);
		glm::vec3* newNormals = &(mesh.normals[VERTICES_PER_TRIANGLE * i]);
		glm::vec2* newTcoords = &(mesh.texcoords[VERTICES_PER_TRIANGLE * i]);

		_getFacePosNormalTcoords(f, aiMesh, newPositions, newNormals, newTcoords);
		
		mesh.bbox.updateWithTriangle(newPositions);
	}
}

//=================================================================================
void ModelLoader::_allocateNewMeshes(const unsigned int numNewMeshes, std::vector<Mesh>& meshes)
{
    if(numNewMeshes)
        meshes.insert(meshes.end(), numNewMeshes, Mesh());
}

//=================================================================================
void ModelLoader::_allocateMesh(Mesh& mesh, const unsigned int numFaces)
{
    mesh.vertices.resize(numFaces * VERTICES_PER_TRIANGLE);
    mesh.normals.resize(numFaces * VERTICES_PER_TRIANGLE);
    mesh.texcoords.resize(numFaces * VERTICES_PER_TRIANGLE);
}

//=================================================================================
void ModelLoader::_assignMeshMaterial(Mesh& mesh, const aiMesh* aimesh)
{
    mesh.materialIndex = aimesh->mMaterialIndex + _numMaterialsPreviouslyLoaded;
}

//=================================================================================
void ModelLoader::_getFacePosNormalTcoords(const aiFace* face, const aiMesh* mesh, glm::vec4* pos, glm::vec3* normal, glm::vec2* tcoords)
{
	GLE_ASSERT(face->mNumIndices == VERTICES_PER_TRIANGLE, "Triangle should have {} vertices", VERTICES_PER_TRIANGLE);

	for(unsigned int i = 0; i< face->mNumIndices; ++i)
    {
        aiVector3D p = mesh->mVertices[face->mIndices[i]];
        aiVector3D n = mesh->mNormals[face->mIndices[i]];
		aiVector3D tc;

        if(mesh->HasTextureCoords(0))
            tc = mesh->mTextureCoords[0][face->mIndices[i]];
        else
			tc = aiVector3D(0, 0, 0);

		pos[i].x = p.x;
		pos[i].y = p.y;
		pos[i].z = p.z;
		pos[i].w = 1.0;

		normal[i].x = n.x;
		normal[i].y = n.y;
		normal[i].z = n.z;

		tcoords[i].x = tc.x;
		tcoords[i].y = tc.y;
    }
}
}}}