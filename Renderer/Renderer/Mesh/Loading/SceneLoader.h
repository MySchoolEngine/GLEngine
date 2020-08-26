#pragma once

#include <Renderer/RendererApi.h>
#include <Renderer/Mesh/Loading/ModelLoader.h>
#include <Renderer/Mesh/Scene.h>

namespace GLEngine::Renderer::Mesh {
//=================================================================================
class RENDERER_API_EXPORT SceneLoader
{
public:
	//filepath - the folder where the model is
	//filename - the actual name of the file
	[[nodiscard]] bool addModelFromFileToScene(const std::filesystem::path& filepath, const std::filesystem::path& filename, std::shared_ptr<Renderer::MeshData::Scene> scene, const glm::mat4& transform = glm::mat4(1));
};
}