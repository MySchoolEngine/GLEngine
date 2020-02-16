#pragma once

#include <Renderer/Animation/IPose.h>

#include <Renderer/Animation/AnimationStructures.h>


namespace GLEngine::Renderer::Animation {
/**
 * Represents sampled value of animation in given time.
 * Should come out of keyframe sampling, IK animation or similar systems
 */
class C_Pose : public I_Pose
{
public:
	C_Pose(std::vector<S_BoneKeyframe>&& keyframe);

	virtual const glm::mat4 GetModelSpaceTransform(int boneID) const override;
	virtual void SetModelSpaceTransform(const glm::mat4& transform, int boneID) override;
	virtual std::vector<glm::mat4> GetModelSpaceTransofrms() const override;
private:
	std::vector<S_BoneKeyframe> m_keyFrame;
};
}