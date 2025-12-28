#include <RendererStdafx.h>

#include <Renderer/Mesh/Loading/ModelLoader.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace GLEngine::Renderer::Mesh {

std::mutex ModelLoader::m_Mutex;

//=================================================================================
namespace {
glm::mat4 _aiMatrixToGlm(const aiMatrix4x4& aiMatrix)
{
	aiMatrix4x4 temp = aiMatrix;
	temp.Inverse();

	glm::mat4 glmMat;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			glmMat[i][j] = temp[i][j];

	return glmMat;
}
} // namespace

//=================================================================================
unsigned int ModelLoader::_numTexturesPreviouslyLoaded	= 0;
unsigned int ModelLoader::_numMaterialsPreviouslyLoaded = 0;
unsigned int ModelLoader::_numMeshesPreviouslyLoaded	= 0;

//=================================================================================
ModelLoader::ModelLoader()
	: _importer(std::make_unique<Assimp::Importer>())
{
}

//=================================================================================
ModelLoader::~ModelLoader() = default;

//=================================================================================
bool ModelLoader::addModelFromFileToScene(const std::filesystem::path&						path,
										  const std::shared_ptr<Renderer::MeshData::Scene>& scene,
										  std::vector<std::filesystem::path>&				textureRegister,
										  const glm::mat4&									sceneTransform)
{
	const auto loadedScene = _tryOpenFile(path);

	if (loadedScene == nullptr)
		return false;

	_loadMaterialsFromAiscene(loadedScene, scene, textureRegister);

	_loadMeshesFromAiScene(loadedScene, scene, sceneTransform);

	_loadLightsFromAiScene(loadedScene, scene);

	const auto parentDir = path.parent_path();
	for (auto& texturePath : textureRegister)
	{
		if (texturePath.is_absolute())
		{
			CORE_LOG(E_Level::Warning, E_Context::Render, "Model {} has texture {} set as absolute path. This would not work if you checkout repository to different path.", path,
					 texturePath);
		}
		if (texturePath.is_relative())
		{
			texturePath = parentDir / texturePath;
			scene->textures.emplace_back(texturePath);
		}
	}

	_numMaterialsPreviouslyLoaded += loadedScene->mNumMaterials;

	_importer->FreeScene();

	return true;
}

//=================================================================================
void ModelLoader::Reset()
{
	_numMaterialsPreviouslyLoaded = _numMeshesPreviouslyLoaded = _numTexturesPreviouslyLoaded = 0;
}

//=================================================================================
void ModelLoader::_loadMaterialsFromAiscene(const aiScene* loadedScene, std::shared_ptr<MeshData::Scene> scene, std::vector<std::filesystem::path>& textureRegister)
{
	for (unsigned int i = 0; i < loadedScene->mNumMaterials; ++i)
	{
		MeshData::Material m;
		_getMaterialColorAttributes(loadedScene->mMaterials[i], m);
		aiString matName;
		loadedScene->mMaterials[i]->Get(AI_MATKEY_NAME, matName);
		m.m_Name							= matName.C_Str();
		const std::filesystem::path texName = _getMaterialDiffuseTextureName(loadedScene->mMaterials[i]);
		m.textureIndex						= _getTextureIndexAndAddToRegister(texName, textureRegister);

		const auto dispTexName = _getMaterialNormalTextureName(loadedScene->mMaterials[i]);
		m.normalTextureIndex   = _getTextureIndexAndAddToRegister(dispTexName, textureRegister);
		scene->materials.push_back(m);
	}
}

//=================================================================================
const aiScene* ModelLoader::_tryOpenFile(const std::filesystem::path& path)
{
	const aiScene* s = _importer->ReadFile(path.generic_string().c_str(),
										   aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_RemoveRedundantMaterials | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);

	if (s == nullptr)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to import {}", path);
		return nullptr;
	}

	return s;
}

//=================================================================================
void ModelLoader::_getMaterialColorAttributes(const aiMaterial* const material, MeshData::Material& mat)
{
	mat.ambient = _getMaterialColorComponent(material, AI_MATKEY_COLOR_AMBIENT);

	mat.diffuse	  = glm::vec4(_getMaterialColorComponent(material, AI_MATKEY_COLOR_DIFFUSE), 0.f);
	mat.diffuse.w = _getMaterialFloatComponent(material, AI_MATKEY_OPACITY);

	mat.specular = _getMaterialColorComponent(material, AI_MATKEY_COLOR_SPECULAR);

	mat.shininess = _getMaterialFloatComponent(material, AI_MATKEY_SHININESS);
}

//=================================================================================
Colours::T_Colour ModelLoader::_getMaterialColorComponent(const aiMaterial* const material, const char* key, unsigned int type, unsigned int index)
{
	if (material)
	{
		aiColor3D c;
		material->Get(key, type, index, c);
		return {c.r, c.g, c.b};
	}

	return {0.f, 0.f, 0.f};
}

//=================================================================================
float ModelLoader::_getMaterialFloatComponent(const aiMaterial* material, const char* key, unsigned type, unsigned index)
{
	if (material)
	{
		float f;
		material->Get(key, type, index, f);
		return f;
	}

	return 0;
}

//=================================================================================
std::filesystem::path ModelLoader::_getMaterialDiffuseTextureName(const aiMaterial* material)
{
	if (material && material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
	{
		aiString tpath;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &tpath);
		return tpath.C_Str();
	}

	return "";
}

//=================================================================================
std::string ModelLoader::_getMaterialNormalTextureName(const aiMaterial* material)
{
	if (material && material->GetTextureCount(aiTextureType_NORMALS) != 0)
	{
		aiString tpath;
		material->GetTexture(aiTextureType_NORMALS, 0, &tpath);
		return tpath.C_Str();
	}

	return "";
}

//=================================================================================
int ModelLoader::_getTextureIndexAndAddToRegister(const std::filesystem::path& name, std::vector<std::filesystem::path>& textureNames)
{
	if (name.empty())
		return -1;

	textureNames.push_back(name);
	return _numTexturesPreviouslyLoaded++;
}

//=================================================================================
void ModelLoader::_loadLightsFromAiScene(const aiScene* loadedScene, std::shared_ptr<MeshData::Scene> scene)
{
	for (unsigned int i = 0; i < loadedScene->mNumLights; ++i)
	{
		MeshData::Light light;
		const auto		lightSource = loadedScene->mLights[i];
		const auto		col			= lightSource->mColorDiffuse;
		light.m_Color				= {col.r, col.g, col.b};
		light.m_name				= std::string(lightSource->mName.C_Str());
		scene->lights.push_back(light);
	}
}

//=================================================================================
void ModelLoader::_loadSkeletonFromAiScene(const aiMesh* aiMesh, MeshData::Mesh& mesh)
{
	if (aiMesh->mNumBones)
	{
		mesh.skeleton.jointIndices.resize(aiMesh->mNumVertices);
		mesh.skeleton.weights.resize(aiMesh->mNumVertices);
		mesh.skeleton.bones.reserve(aiMesh->mNumBones);
		for (unsigned int i = 0; i < aiMesh->mNumBones; ++i)
		{
			const auto* aiBone = aiMesh->mBones[i];
			auto		boneID = i;
			std::string boneName(aiBone->mName.data);
			JointID		nodeID(boneName);
			if (mesh.skeleton.GetBoneIndex(nodeID) == MeshData::SkeletonData::BadIndex)
			{
				boneID							  = i;
				mesh.skeleton.boneMapping[nodeID] = boneID;
				mesh.skeleton.bones.push_back({nodeID, _aiMatrixToGlm(aiBone->mOffsetMatrix)});
			}
			else
			{
				boneID = mesh.skeleton.boneMapping[nodeID];
			}
			for (unsigned int j = 0; j < aiBone->mNumWeights; ++j)
			{
				const aiVertexWeight& aiVertexVewight = aiBone->mWeights[j];
				mesh.skeleton.AddBoneData(aiVertexVewight.mVertexId, boneID, aiVertexVewight.mWeight);
			}
		}
	}
}

//=================================================================================
void ModelLoader::_loadMeshesFromAiScene(const aiScene* loadedScene, std::shared_ptr<MeshData::Scene> scene, const glm::mat4& sceneTransform)
{
	const aiNode* currentNode	   = loadedScene->mRootNode;
	glm::mat4	  currentTransform = sceneTransform * _aiMatrixToGlm(loadedScene->mRootNode->mTransformation);

	while (currentNode != nullptr)
	{
		_pushNodeChildrenOnStacks(currentNode, currentTransform);

		_loadNodeMeshes(currentNode, currentTransform, loadedScene->mMeshes, scene->meshes);

		// todo index
		//_loadArmatureData(currentNode, currentTransform, scene->meshes[0]);

		_getNextNodeAndTransform(currentNode, currentTransform);
	}
}

//=================================================================================
void ModelLoader::_pushNodeChildrenOnStacks(const aiNode* node, const glm::mat4& nodeWorldTransform)
{
	const unsigned int numChildren = node->mNumChildren;

	for (unsigned int i = 0; i < numChildren; ++i)
	{
		_nodeStack.push(node->mChildren[i]);
		_transformStack.push(nodeWorldTransform * _aiMatrixToGlm(node->mTransformation));
	}
}

//=================================================================================
void ModelLoader::_getNextNodeAndTransform(const aiNode*& node, glm::mat4& transform)
{
	if (_nodeStack.empty())
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
void ModelLoader::_loadNodeMeshes(const aiNode* node, const glm::mat4& nodeTransform, aiMesh** const aiMeshes, std::vector<MeshData::Mesh>& meshes)
{
	_allocateNewMeshes(node->mNumMeshes, meshes);

	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		const unsigned int meshIndex = node->mMeshes[i];
		_loadSingleMeshFromAimesh(aiMeshes[meshIndex], meshes[_numMeshesPreviouslyLoaded + i]);
		_loadSkeletonFromAiScene(aiMeshes[meshIndex], meshes[_numMeshesPreviouslyLoaded + i]);

		meshes[_numMeshesPreviouslyLoaded + i].modelMatrix = nodeTransform;
		meshes[_numMeshesPreviouslyLoaded + i].m_name	   = std::string(node->mName.C_Str());
	}

	_numMeshesPreviouslyLoaded += node->mNumMeshes;
}

//=================================================================================
void ModelLoader::_loadSingleMeshFromAimesh(const aiMesh* aiMesh, MeshData::Mesh& mesh)
{
	_allocateMesh(mesh, aiMesh->mNumFaces);
	_assignMeshMaterial(mesh, aiMesh);

	for (unsigned int i = 0; i < aiMesh->mNumFaces; ++i)
	{
		aiFace* f = &aiMesh->mFaces[i];

		glm::vec3* newPositions	 = &(mesh.vertices[VERTICES_PER_TRIANGLE * i]);
		glm::vec3* newNormals	 = &(mesh.normals[VERTICES_PER_TRIANGLE * i]);
		glm::vec2* newTcoords	 = &(mesh.texcoords[VERTICES_PER_TRIANGLE * i]);
		glm::vec3* newTangents	 = &(mesh.tangent[VERTICES_PER_TRIANGLE * i]);
		glm::vec3* newBitangents = &(mesh.bitangent[VERTICES_PER_TRIANGLE * i]);

		_getFacePosNormalTcoords(f, aiMesh, newPositions, newNormals, newTcoords, newTangents, newBitangents);

		mesh.bbox.updateWithTriangle(newPositions);
	}
}

//=================================================================================
void ModelLoader::_allocateNewMeshes(const unsigned int numNewMeshes, std::vector<MeshData::Mesh>& meshes)
{
	if (numNewMeshes)
		meshes.insert(meshes.end(), numNewMeshes, MeshData::Mesh());
}

//=================================================================================
void ModelLoader::_allocateMesh(MeshData::Mesh& mesh, const unsigned int numFaces)
{
	mesh.vertices.resize(static_cast<std::size_t>(numFaces) * VERTICES_PER_TRIANGLE);
	mesh.normals.resize(static_cast<std::size_t>(numFaces) * VERTICES_PER_TRIANGLE);
	mesh.texcoords.resize(static_cast<std::size_t>(numFaces) * VERTICES_PER_TRIANGLE);
	mesh.tangent.resize(static_cast<std::size_t>(numFaces) * VERTICES_PER_TRIANGLE);
	mesh.bitangent.resize(static_cast<std::size_t>(numFaces) * VERTICES_PER_TRIANGLE);
}

//=================================================================================
void ModelLoader::_assignMeshMaterial(MeshData::Mesh& mesh, const aiMesh* aimesh)
{
	mesh.materialIndex = aimesh->mMaterialIndex + _numMaterialsPreviouslyLoaded;
}

//=================================================================================
void ModelLoader::_getFacePosNormalTcoords(const aiFace* face,
										   const aiMesh* mesh,
										   glm::vec3*	 pos,
										   glm::vec3*	 normal,
										   glm::vec2*	 tcoords,
										   glm::vec3*	 ttangents,
										   glm::vec3*	 tbitangents)
{
	GLE_ASSERT(face->mNumIndices == VERTICES_PER_TRIANGLE, "Triangle should have {} vertices", VERTICES_PER_TRIANGLE);

	for (unsigned int i = 0; i < face->mNumIndices; ++i)
	{
		aiVector3D p = mesh->mVertices[face->mIndices[i]];
		aiVector3D n = mesh->mNormals[face->mIndices[i]];
		aiVector3D tc;
		aiVector3D tng = mesh->mTangents[face->mIndices[i]];
		aiVector3D btg = mesh->mBitangents[face->mIndices[i]];

		if (mesh->HasTextureCoords(0))
			tc = mesh->mTextureCoords[0][face->mIndices[i]];
		else
			tc = aiVector3D(0, 0, 0);

		pos[i].x = p.x;
		pos[i].y = p.y;
		pos[i].z = p.z;

		normal[i].x = n.x;
		normal[i].y = n.y;
		normal[i].z = n.z;

		tcoords[i].x = tc.x;
		tcoords[i].y = tc.y;

		ttangents[i].x = tng.x;
		ttangents[i].y = tng.y;
		ttangents[i].z = tng.z;

		tbitangents[i].x = btg.x;
		tbitangents[i].y = btg.y;
		tbitangents[i].z = btg.z;
	}
}

//=================================================================================
std::mutex& ModelLoader::GetMutex()
{
	return m_Mutex;
}

//=================================================================================
void ModelLoader::_loadArmatureData(const aiNode* currentNode, const glm::mat4& currentTransform, MeshData::Mesh& mesh)
{
}

} // namespace GLEngine::Renderer::Mesh