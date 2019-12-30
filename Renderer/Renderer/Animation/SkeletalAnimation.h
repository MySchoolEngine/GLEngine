#pragma once

#include <Renderer/RendererApi.h>

#include <algorithm>

namespace GLEngine::Renderer::Animation {
struct S_Timestamp
{
	S_Timestamp();
	S_Timestamp(float timestamp);

	bool operator>=(const S_Timestamp& other) const;
private:
	static float Clamp(float f);
	float m_Timestamp;
};

struct S_BoneKeyframe
{
	S_BoneKeyframe() = default;
	S_BoneKeyframe(const glm::mat4& matrix, S_Timestamp timestamp);
	S_Timestamp		m_Timestamp;
	glm::vec3			m_Transform;
	glm::quat			m_Rotation;
};

// todo: move ctor
class C_BoneTimeline
{
public:
	C_BoneTimeline() = default;
	C_BoneTimeline(std::size_t numTimestamps);

	void AddBoneKeyFrame(std::size_t index, S_BoneKeyframe&& keyframe);

private:
	// ideally BST because intervals do not overlap
	// but we can use vector for now, as we will usually add 
	// keyframes in order
	std::vector<S_BoneKeyframe> m_Timeline;
};

// for each bone, we want individual timeline
class RENDERER_API_EXPORT C_SkeletalAnimation {
public:
	C_SkeletalAnimation(std::size_t numBones);
	//std::vector<S_SkeletalJointTransform> GetTransform(const S_Timestamp timestamp);
	void SetBoneTimeline(std::size_t index, C_BoneTimeline&& timeline);
private:
	std::vector<C_BoneTimeline> m_Timelines;
};

}