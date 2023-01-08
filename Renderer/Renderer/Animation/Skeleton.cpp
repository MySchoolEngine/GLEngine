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

//=================================================================================
const S_Joint& C_Skeleton::GetRoot() const
{
	return *m_Root.get();
}

//=================================================================================
JointID::JointID(const std::string& name)
	: m_Name(name)
	, m_ID(std::hash<std::string>{}(name))
{
}

//=================================================================================
bool JointID::operator==(const JointID& other) const
{
	return m_ID == other.m_ID;
}

//=================================================================================
bool JointID::operator!=(const JointID& other) const
{
	return m_ID != other.m_ID;
}

//=================================================================================
bool JointID::operator<(const JointID& other) const
{
	return m_ID < other.m_ID;
}

} // namespace GLEngine::Renderer::Animation