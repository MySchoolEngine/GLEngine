#pragma once

#include <Renderer/Animation/Joint.h>
#include <Renderer/RendererApi.h>

#include <glm/gtc/quaternion.hpp>

namespace GLEngine::Renderer {
namespace MeshData {
struct SkeletonData;
}

class RENDERER_API_EXPORT C_Skeleton final {
public:
	C_Skeleton();
	C_Skeleton(const MeshData::SkeletonData& skeletalData);
	[[nodiscard]] std::size_t GetNumBones() const;
	void					  ConverToLS(std::vector<glm::mat4>& poseData) const;
	const S_Joint&			  GetRoot() const;
	S_Joint&				  GetRoot();
	T_BoneIndex				  GetRootIndex() const;
	S_Joint*				  GetJoint(const T_BoneIndex index);
	const S_Joint* const	  GetJoint(const T_BoneIndex index) const;
	[[nodiscard]] T_BoneIndex GetJointIndex(JointID jointId) const;

private:
	std::vector<S_Joint> m_Bones;
	T_BoneIndex			 m_Root = s_BoneBadIndex;

	friend class C_ColladaLoader; // :(
};

} // namespace GLEngine::Renderer