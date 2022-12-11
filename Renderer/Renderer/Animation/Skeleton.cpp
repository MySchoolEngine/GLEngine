#include <RendererStdafx.h>

#include <Renderer/Animation/Skeleton.h>

namespace GLEngine::Renderer {

//=================================================================================
C_Skeleton::C_Skeleton() = default;

//=================================================================================
std::size_t C_Skeleton::GetNumBones() const
{
	return m_NumBones;
}

//=================================================================================
void C_Skeleton::SetNumBonse(std::size_t num)
{
	m_NumBones = num;
}

//=================================================================================
void C_Skeleton::ApplyPoseToBones(std::vector<glm::mat4>& poseData) const
{
	m_Root->ApplyPoseToJoints(poseData, glm::mat4(1.f));
}

} // namespace GLEngine::Renderer::Animation