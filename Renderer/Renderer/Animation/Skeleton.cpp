#include <RendererStdafx.h>

#include <Renderer/Animation/Skeleton.h>
#include <Renderer/Mesh/Scene.h>

namespace GLEngine::Renderer {

//=================================================================================
C_Skeleton::C_Skeleton() = default;

//=================================================================================
std::size_t C_Skeleton::GetNumBones() const
{
	return m_Bones.size();
}

//=================================================================================
void C_Skeleton::ConverToLS(std::vector<glm::mat4>& poseData) const
{
	GLE_ASSERT(m_Root != s_BoneBadIndex, "Using uninitalized skeleton for evaluation");
	GLE_ASSERT(m_Bones.size() == poseData.size(), "Wrong buffer size");
	std::function<void(T_BoneIndex id, const glm::mat4& parentTransform)> toLocalSpace;
	toLocalSpace = [&](T_BoneIndex id, const glm::mat4& parentTransform) {
		const auto& currentLocalPose = poseData[id];
		const auto& currentPose		 = parentTransform * currentLocalPose;
		for (const T_BoneIndex child : m_Bones[id].m_Children)
		{
			toLocalSpace(child, currentPose);
		}
		const auto& newPose = currentPose * m_Bones[id].m_InverseBindTransform;
		poseData[id]		= newPose;
	};
	toLocalSpace(m_Root, glm::mat4(1.f));
}

//=================================================================================
const S_Joint& C_Skeleton::GetRoot() const
{
	GLE_ASSERT(m_Root != s_BoneBadIndex, "Using uninitalized skeleton for evaluation");
	return m_Bones[m_Root];
}

//=================================================================================
S_Joint& C_Skeleton::GetRoot()
{
	GLE_ASSERT(m_Root != s_BoneBadIndex, "Using uninitalized skeleton for evaluation");
	return m_Bones[m_Root];
}

//=================================================================================
C_Skeleton::C_Skeleton(const MeshData::SkeletonData& skeletalData)
{
	m_Bones.reserve(skeletalData.bones.size());
}

//=================================================================================
S_Joint* C_Skeleton::GetJoint(const T_BoneIndex index)
{
	return &m_Bones[index];
}

//=================================================================================
const S_Joint* const C_Skeleton::GetJoint(const T_BoneIndex index) const
{
	return &m_Bones[index];
}

//=================================================================================
T_BoneIndex C_Skeleton::GetJointIndex(JointID jointId) const
{
	for (T_BoneIndex i = 0; i < m_Bones.size(); i++) {
		if (m_Bones[i].m_Id == jointId)
			return i;
	}
	return s_BoneBadIndex;
}

//=================================================================================
T_BoneIndex C_Skeleton::GetRootIndex() const
{
	return m_Root;
}

} // namespace GLEngine::Renderer::Animation