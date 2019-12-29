#pragma once

#include <GLRenderer/MeshLoading/ModelLoader.h>
#include <GLRenderer/MeshLoading/Scene.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>

namespace pugi
{
class xml_node;
}

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
	bool addModelFromDAEFileToScene(const char* filepath, const char* filename, std::shared_ptr<C_StaticMeshResource>& mesh, std::string& textureName, Renderer::Animation::C_Skeleton& skeleton, const glm::mat4& transform = glm::mat4(1));
private:
	void LoadJoints(std::vector<std::string>& jointNames, const pugi::xml_node& skinXML) const;
	void LoadJointsInvMatrices(const std::vector<std::string>& jointNames, std::map<std::string, Renderer::Animation::S_Joint>& joints, const pugi::xml_node& skinXML, const glm::mat4& normalizinMatrix) const;
	glm::mat4 ParseTranslation(const pugi::xml_node& node);
	bool ParseChildrenJoints(Renderer::Animation::S_Joint& parent, const pugi::xml_node& xmlParent, const std::map<std::string, Renderer::Animation::S_Joint>& joints);
};
}