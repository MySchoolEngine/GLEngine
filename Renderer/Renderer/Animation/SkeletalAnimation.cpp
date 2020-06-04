#include <RendererStdafx.h>

#include <Renderer/Animation/SkeletalAnimation.h>

#include <Renderer/Animation/Pose.h>

namespace GLEngine::Renderer::Animation {
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
C_Pose C_SkeletalAnimation::GetPose(const S_Timestamp timestamp) const
{
	std::vector<S_BoneKeyframe> ret;
	ret.resize(m_Timelines.size());
	std::size_t index = 0;
	for (const auto& timeline : m_Timelines)
	{
		ret[index] = timeline.Sample(timestamp);

		++index;
	}
	return C_Pose(std::move(ret));
}

}