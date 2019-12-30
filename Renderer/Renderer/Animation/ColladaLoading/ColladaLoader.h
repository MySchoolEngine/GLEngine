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
class C_BoneTimeline;
}

namespace GLEngine::Renderer::Animation {

class RENDERER_API_EXPORT C_ColladaLoader {
public:
	void Reset();

	//filepath - the folder where the model is
	//filename - the actual name of the file
	bool addModelFromDAEFileToScene(const char* filepath, const char* filename, MeshData::Mesh& mesh, std::string& textureName, 
		C_Skeleton& skeleton,
		C_SkeletalAnimation& animation, 
		const glm::mat4& transform = glm::mat4(1));
private:
	void LoadJoints(const pugi::xml_node& skinXML);
	void LoadJointsInvMatrices(std::map<std::string, S_Joint>& joints, const pugi::xml_node& skinXML, const glm::mat4& normalizinMatrix) const;
	glm::mat4 ParseTranslation(const pugi::xml_node& node);
	bool ParseChildrenJoints(S_Joint& parent, const pugi::xml_node& xmlParent, const std::map<std::string, S_Joint>& joints);

	std::size_t GetBoneId(const std::string& name) const;
	C_BoneTimeline LoadBoneTimeline(const pugi::xml_node& node) const;

	std::vector<std::string> m_JointNames;
};

}