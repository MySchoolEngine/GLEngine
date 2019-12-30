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

struct S_SkeletalJointTransform
{
	S_SkeletalJointTransform() = default;
	S_SkeletalJointTransform(const glm::mat4& matrix);
	glm::vec3 m_Transform;
	glm::quat m_Rotation;
};

class C_SkeletalAnimationKeyframe {
public:
	C_SkeletalAnimationKeyframe(S_Timestamp timestamp, std::size_t numBones);

	void AddJointTransform(std::size_t index, S_SkeletalJointTransform&& transform);

	const S_Timestamp& GetTimestamp() const;
private:
	std::vector<S_SkeletalJointTransform> m_Transforms;
	S_Timestamp m_Timestamp;
};

class RENDERER_API_EXPORT C_SkeletalAnimation {
public:
	C_SkeletalAnimation();
	//std::vector<S_SkeletalJointTransform> GetTransform(const S_Timestamp timestamp);
	void AddKeyframe(C_SkeletalAnimationKeyframe&& keyframe);
private:
	// ideally BST because intervals do not overlap
	// but we can use vector for now, as we will usually add 
	// keyframes in order
	std::vector<C_SkeletalAnimationKeyframe> m_Keyframes;
};

}