#pragma once

#include <Renderer/Animation/AnimationStructures.h>
#include <Renderer/Animation/Pose.h>
#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer::Animation {

// for each bone, we want individual timeline
class RENDERER_API_EXPORT C_SkeletalAnimation {
public:
	explicit C_SkeletalAnimation(std::size_t numBones);
	[[nodiscard]] C_Pose GetPose(const GLEngine::Animation::S_Timestamp timestamp) const;
	void				 SetBoneTimeline(std::size_t index, C_BoneTimeline&& timeline);

private:
	std::vector<C_BoneTimeline> m_Timelines;
};

} // namespace GLEngine::Renderer::Animation