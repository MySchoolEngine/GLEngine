#include <RendererStdafx.h>

#include <Renderer/Animation/Pose.h>

namespace GLEngine::Renderer {

//=================================================================================
C_Pose::C_Pose(std::vector<S_BoneKeyframe>&& keyframe)
	: m_keyFrame(keyframe)
{
}

//=================================================================================
const glm::mat4 C_Pose::GetModelSpaceTransform(int boneID) const
{
	return m_keyFrame[boneID].GetTransformationMatrix();
}

//=================================================================================
void C_Pose::SetModelSpaceTransform(const glm::mat4& transform, int boneID)
{
	m_keyFrame[boneID] = S_BoneKeyframe(transform, 0.f);
}

//=================================================================================
std::vector<glm::mat4> C_Pose::GetModelSpaceTransofrms() const
{
	std::vector<glm::mat4> transofrms;
	transofrms.resize(m_keyFrame.size());

	std::transform(m_keyFrame.begin(), m_keyFrame.end(), transofrms.begin(), [](const S_BoneKeyframe& keyFrame) { return keyFrame.GetTransformationMatrix(); });

	return transofrms;
}

} // namespace GLEngine::Renderer