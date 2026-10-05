#include <RendererStdafx.h>

#include <Renderer/Animation/Pose.h>

namespace GLEngine::Renderer {

//=================================================================================
C_Pose::C_Pose(std::vector<S_BoneKeyframe>&& keyframe)
	: m_KeyFrame(keyframe)
{
}

//=================================================================================
const glm::mat4 C_Pose::GetLocalSpaceTransform(int boneID) const
{
	return m_KeyFrame[boneID].GetTransformationMatrix();
}

//=================================================================================
void C_Pose::SetLocalSpaceTransform(const glm::mat4& transform, int boneID)
{
	m_KeyFrame[boneID] = S_BoneKeyframe(transform, 0.f);
}

//=================================================================================
std::vector<glm::mat4> C_Pose::GetMSTransofrms() const
{
	std::vector<glm::mat4> transofrms;
	transofrms.resize(m_KeyFrame.size());

	std::transform(m_KeyFrame.begin(), m_KeyFrame.end(), transofrms.begin(), [](const S_BoneKeyframe& keyFrame) { return keyFrame.GetTransformationMatrix(); });

	return transofrms;
}

//=================================================================================
void C_Pose::Blend(const C_Pose& other, float weight)
{
	GLE_ASSERT(m_KeyFrame.size() == other.m_KeyFrame.size(), "Number of keyframes in both poses have to match.");
	for (unsigned int i = 0; i < m_KeyFrame.size(); ++i) {

	}
}

} // namespace GLEngine::Renderer