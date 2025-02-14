#pragma once

#include <Renderer/Animation/AnimationStructures.h>
#include <Renderer/Animation/IPose.h>

namespace GLEngine::Renderer {
/**
 * Represents sampled value of animation in given time.
 * Should come out of keyframe sampling, IK animation or similar systems
 */
class C_Pose : public I_Pose {
public:
	explicit C_Pose(std::vector<S_BoneKeyframe>&& keyframe);

	[[nodiscard]] const glm::mat4		 GetLocalSpaceTransform(int boneID) const override;
	void								 SetLocalSpaceTransform(const glm::mat4& transform, int boneID) override;
	[[nodiscard]] std::vector<glm::mat4> GetLocalSpaceTransofrms() const override;

	void Blend(const C_Pose& other, float weight);

private:
	std::vector<S_BoneKeyframe> m_KeyFrame;
};
} // namespace GLEngine::Renderer