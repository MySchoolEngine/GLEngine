#pragma once

//#include "TextureLoader.hpp"
#include <GLRenderer/MeshLoading/ModelLoader.h>
#include <GLRenderer/MeshLoading/Scene.h>

namespace GLEngine {
namespace GLRenderer {
namespace Mesh {
//=================================================================================
class SceneLoader
{
public:
	//filepath - the folder where the model is
	//filename - the actual name of the file
	bool addModelFromFileToScene(const char* filepath, const char* filename, std::shared_ptr<Scene> scene, const glm::mat4& transform = glm::mat4(1));
};
}}}