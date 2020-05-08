#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Animation/AnimationStructures.h>

#include <Renderer/Animation/IPose.h>
#include <Renderer/Animation/Pose.h>

#include <algorithm>

namespace GLEngine::Renderer::Animation {

// for each bone, we want individual timeline
class RENDERER_API_EXPORT C_SkeletalAnimation {
public:
	explicit C_SkeletalAnimation(std::size_t numBones);
	[[nodiscard]] C_Pose GetPose(const S_Timestamp timestamp) const;
	void SetBoneTimeline(std::size_t index, C_BoneTimeline&& timeline);
private:
#pragma warning(push)
#pragma warning( disable : 4251)
	std::vector<C_BoneTimeline> m_Timelines;
#pragma warning(pop)
};

}