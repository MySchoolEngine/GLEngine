#include <RendererStdafx.h>

#include <Renderer/Animation/SkeletalAnimation.h>

#include <Core/CoreMacros.h>

namespace GLEngine::Renderer::Animation {

//=================================================================================
// S_Timestamp
//=================================================================================
S_Timestamp::S_Timestamp(float timestamp) 
	: m_Timestamp(S_Timestamp::Clamp(timestamp))
{

}

//=================================================================================
S_Timestamp::S_Timestamp() 
	: m_Timestamp(0.f)
{

}

//=================================================================================
float S_Timestamp::Clamp(float f)
{
	return std::clamp(f, 0.0f, 1.0f);
}

//=================================================================================
bool S_Timestamp::operator>=(const S_Timestamp& other) const
{
	return m_Timestamp >= other.m_Timestamp;
}

//=================================================================================
// S_BoneKeyframe
//=================================================================================
S_BoneKeyframe::S_BoneKeyframe(const glm::mat4& matrix, S_Timestamp timestamp)
	: m_Transform(matrix[3][0], matrix[3][1], matrix[3][2])
	, m_Rotation(glm::quat_cast(matrix))
	, m_Timestamp(timestamp)
{

}

//=================================================================================
// C_BoneTimeline
//=================================================================================
C_BoneTimeline::C_BoneTimeline(std::size_t numTimestamps)
{
	m_Timeline.resize(numTimestamps);
}

//=================================================================================
void C_BoneTimeline::AddBoneKeyFrame(std::size_t index, S_BoneKeyframe&& keyframe)
{
	GLE_ASSERT(keyframe.m_Timestamp >= m_Timeline.back().m_Timestamp, "Adding keyframes out of order not suppoerted");
	m_Timeline[index] = (std::move(keyframe));
}

//=================================================================================
// C_SkeletalAnimation
//=================================================================================
C_SkeletalAnimation::C_SkeletalAnimation(std::size_t numBones)
{
	m_Timelines.resize(numBones);
}

//=================================================================================
void C_SkeletalAnimation::SetBoneTimeline(std::size_t index, C_BoneTimeline&& timeline)
{
	m_Timelines[index] = std::move(timeline);
}

}