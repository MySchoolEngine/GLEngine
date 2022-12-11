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

	[[nodiscard]] virtual const glm::mat4		 GetModelSpaceTransform(int boneID) const override;
	virtual void								 SetModelSpaceTransform(const glm::mat4& transform, int boneID) override;
	[[nodiscard]] virtual std::vector<glm::mat4> GetModelSpaceTransofrms() const override;

private:
	std::vector<S_BoneKeyframe> m_keyFrame;
};
} // namespace GLEngine::Renderer