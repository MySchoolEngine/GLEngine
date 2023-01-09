#include <RendererStdafx.h>

#include <Renderer/Animation/Pose.h>

namespace GLEngine::Renderer {

//=================================================================================
C_Pose::C_Pose(std::vector<S_BoneKeyframe>&& keyframe)
	: m_keyFrame(keyframe)
{
}

//=================================================================================
const glm::mat4 C_Pose::GetLocalSpaceTransform(int boneID) const
{
	return m_keyFrame[boneID].GetTransformationMatrix();
}

//=================================================================================
void C_Pose::SetLocalSpaceTransform(const glm::mat4& transform, int boneID)
{
	m_keyFrame[boneID] = S_BoneKeyframe(transform, 0.f);
}

//=================================================================================
std::vector<glm::mat4> C_Pose::GetLocalSpaceTransofrms() const
{
	std::vector<glm::mat4> transofrms;
	transofrms.resize(m_keyFrame.size());

	std::transform(m_keyFrame.begin(), m_keyFrame.end(), transofrms.begin(), [](const S_BoneKeyframe& keyFrame) { return keyFrame.GetTransformationMatrix(); });

	return transofrms;
}

//=================================================================================
void C_Pose::Blend(const C_Pose& other, float weight)
{
	GLE_ASSERT(m_keyFrame.size() == other.m_keyFrame.size(), "Number of keyframes in both poses have to match.");
	for (unsigned int i = 0; i < m_keyFrame.size(); ++i) {

	}
}

} // namespace GLEngine::Renderer