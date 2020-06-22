#pragma once

#include <GLRenderer/MeshLoading/ModelLoader.h>
#include <Renderer/Mesh/Scene.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>

namespace GLEngine::GLRenderer::Mesh {
//=================================================================================
class SceneLoader
{
public:
	//filepath - the folder where the model is
	//filename - the actual name of the file
	bool addModelFromFileToScene(const std::filesystem::path& filepath, const std::filesystem::path& filename, std::shared_ptr<Renderer::MeshData::Scene> scene, const glm::mat4& transform = glm::mat4(1));
};
}