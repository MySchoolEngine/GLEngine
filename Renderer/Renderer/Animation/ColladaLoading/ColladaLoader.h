#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Mesh/Scene.h>

namespace pugi
{
class xml_node;
}

namespace GLEngine::Renderer::Animation
{
class C_SkeletalAnimation;
class C_Skeleton;
struct S_Joint;
}

namespace GLEngine::Renderer::Animation {

class RENDERER_API_EXPORT C_ColladaLoader {
public:
	void Reset();

	//filepath - the folder where the model is
	//filename - the actual name of the file
	bool addModelFromDAEFileToScene(const char* filepath, const char* filename, Renderer::MeshData::Mesh& mesh, std::string& textureName, Renderer::Animation::C_Skeleton& skeleton,
		Renderer::Animation::C_SkeletalAnimation& animation, const glm::mat4& transform = glm::mat4(1));
private:
	void LoadJoints(std::vector<std::string>& jointNames, const pugi::xml_node& skinXML) const;
	void LoadJointsInvMatrices(const std::vector<std::string>& jointNames, std::map<std::string, Renderer::Animation::S_Joint>& joints, const pugi::xml_node& skinXML, const glm::mat4& normalizinMatrix) const;
	glm::mat4 ParseTranslation(const pugi::xml_node& node);
	bool ParseChildrenJoints(Renderer::Animation::S_Joint& parent, const pugi::xml_node& xmlParent, const std::map<std::string, Renderer::Animation::S_Joint>& joints);
};

}