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
bool S_Timestamp::operator>(const S_Timestamp& other) const
{
	return m_Timestamp > other.m_Timestamp;
}

//=================================================================================
S_Timestamp S_Timestamp::operator-(const S_Timestamp& rhs) const
{
	return S_Timestamp(m_Timestamp - rhs.m_Timestamp);
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
S_BoneKeyframe::S_BoneKeyframe(const glm::vec3& transformation, const glm::quat& rotation, S_Timestamp timestamp)
	: m_Transform(transformation)
	, m_Rotation(rotation)
	, m_Timestamp(timestamp)
{

}

//=================================================================================
glm::mat4 S_BoneKeyframe::GetTransformationMatrix() const
{
	return glm::mat4(1.0f);
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
S_BoneKeyframe C_BoneTimeline::Sample(S_Timestamp timestamp) const
{
	GLE_ASSERT(m_Timeline.size() > 0, "There is no keyframe in the animation");
	// find first keyframe >= to given timestamp
	const auto& nextKeyframeIt = std::find_if(m_Timeline.begin(), m_Timeline.end(),
		[timestamp](const S_BoneKeyframe& it) { return it.m_Timestamp > timestamp; });
	// we are after last keyframe
	if (nextKeyframeIt == m_Timeline.end())
	{
		return m_Timeline[m_Timeline.size() - 1];
	}
	// we are before keyframe
	else if (nextKeyframeIt == m_Timeline.begin())
	{
		return *nextKeyframeIt;
	}
	// so we are in between two keyframes, so we need to interpolate

	// take this and previous one and interpolate between them
	const auto& previousKeyframe = *(nextKeyframeIt - 1);
	const auto& nextKeyframe = *(nextKeyframeIt);
	// found on which point between them are we
	const auto intervalLen = nextKeyframe.m_Timestamp - previousKeyframe.m_Timestamp;
	const auto timeInInterval = timestamp - previousKeyframe.m_Timestamp;
	const auto percentageProgress = timeInInterval.GetValue() / intervalLen.GetValue();
	// return result
	return S_BoneKeyframe(
		glm::mix(previousKeyframe.m_Transform, nextKeyframe.m_Transform, percentageProgress), 
		glm::slerp(previousKeyframe.m_Rotation, nextKeyframe.m_Rotation, percentageProgress),
		timestamp);
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

//=================================================================================
std::vector<S_BoneKeyframe> C_SkeletalAnimation::GetTransform(const S_Timestamp timestamp) const
{
	std::vector<S_BoneKeyframe> ret;
	ret.resize(m_Timelines.size());
	std::size_t index = 0;
	for (const auto& timeline : m_Timelines)
	{
		ret[index] = timeline.Sample(timestamp);

		++index;
	}
	return ret;
}

}