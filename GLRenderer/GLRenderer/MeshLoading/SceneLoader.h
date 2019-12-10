#pragma once

#include <GLRenderer/MeshLoading/ModelLoader.h>
#include <GLRenderer/MeshLoading/Scene.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>

namespace GLEngine::GLRenderer::Mesh {
//=================================================================================
class SceneLoader
{
public:
	//filepath - the folder where the model is
	//filename - the actual name of the file
	bool addModelFromFileToScene(const char* filepath, const char* filename, std::shared_ptr<Scene> scene, const glm::mat4& transform = glm::mat4(1));


	//filepath - the folder where the model is
	//filename - the actual name of the file
	bool addModelFromDAEFileToScene(const char* filepath, const char* filename, std::shared_ptr<C_StaticMeshResource>& mesh, std::string& textureName, const glm::mat4& transform = glm::mat4(1));
};
}