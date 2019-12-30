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
// S_SkeletalJointTransform
//=================================================================================
S_SkeletalJointTransform::S_SkeletalJointTransform(const glm::mat4& matrix)
	: m_Transform(matrix[3][0], matrix[3][1], matrix[3][2])
	, m_Rotation(glm::quat_cast(matrix))
{

}

//=================================================================================
// C_SkeletalAnimationKeyframe
//=================================================================================
C_SkeletalAnimationKeyframe::C_SkeletalAnimationKeyframe(S_Timestamp timestamp, std::size_t numBones)
	: m_Timestamp(timestamp)
{
	m_Transforms.resize(numBones);
}

//=================================================================================
const S_Timestamp& C_SkeletalAnimationKeyframe::GetTimestamp() const
{
	return m_Timestamp;
}

//=================================================================================
void C_SkeletalAnimationKeyframe::AddJointTransform(std::size_t index, S_SkeletalJointTransform&& transform)
{
	m_Transforms[index] = std::move(transform);
}

//=================================================================================
// C_SkeletalAnimation
//=================================================================================
C_SkeletalAnimation::C_SkeletalAnimation()
{

}

//=================================================================================
void C_SkeletalAnimation::AddKeyframe(C_SkeletalAnimationKeyframe&& keyframe)
{
	GLE_ASSERT(keyframe.GetTimestamp() >= m_Keyframes.back().GetTimestamp(), "Adding keyframes out of order is not allowed now");
	m_Keyframes.emplace_back(std::move(keyframe));
}
}