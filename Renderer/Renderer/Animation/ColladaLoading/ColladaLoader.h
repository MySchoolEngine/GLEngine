#pragma once

#include <Renderer/Mesh/Scene.h>
#include <Renderer/RendererApi.h>

namespace pugi {
class xml_node;
}


namespace GLEngine::Renderer::MeshData {
struct Mesh;
struct AnimationData;
} // namespace GLEngine::Renderer::MeshData

namespace GLEngine::Renderer::Animation {
class C_SkeletalAnimation;
class C_Skeleton;
struct S_Joint;
class C_BoneTimeline;
} // namespace GLEngine::Renderer::Animation

namespace GLEngine::Renderer::Animation {

class RENDERER_API_EXPORT C_ColladaLoader {
public:
	void Reset();

	// filepath - the folder where the model is
	// filename - the actual name of the file
	[[nodiscard]] bool addModelFromDAEFileToScene(const char*			   filepath,
												  const char*			   filename,
												  MeshData::Mesh&		   mesh,
												  std::string&			   textureName,
												  C_Skeleton&			   skeleton,
												  C_SkeletalAnimation&	   animation,
												  MeshData::AnimationData& animData,
												  glm::mat4&			   transform);

private:
	void LoadAnimData(const pugi::xml_node& skinXML, std::vector<glm::ivec3>& jointIndices, std::vector<glm::vec3>& weights);
	void LoadJoints(const pugi::xml_node& skinXML);
	void LoadJointsInvMatrices(std::map<std::string, S_Joint>& joints, const pugi::xml_node& skinXML, const glm::mat4& normalizinMatrix) const;
	bool ParseChildrenJoints(S_Joint& parent, const pugi::xml_node& xmlParent, const std::map<std::string, S_Joint>& joints);

	int			   GetBoneId(const std::string_view& name) const;
	C_BoneTimeline LoadBoneTimeline(const pugi::xml_node& node, const glm::mat4& normalizinMatrix) const;

	std::vector<std::string> m_JointNames;
};

} // namespace GLEngine::Renderer::Animation