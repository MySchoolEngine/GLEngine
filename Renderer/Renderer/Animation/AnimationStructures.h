#pragma once

#include <Renderer/RendererApi.h>

#include <glm/gtc/quaternion.hpp>

namespace GLEngine::Renderer::Animation {

struct S_Timestamp
{
	S_Timestamp();
	RENDERER_API_EXPORT S_Timestamp(float timestamp);

	bool operator>=(const S_Timestamp& other) const;
	bool operator>(const S_Timestamp& other) const;
	S_Timestamp operator-(const S_Timestamp& rhs) const;
	S_Timestamp operator+(const S_Timestamp& rhs) const;
	float GetValue() const { return m_Timestamp; }
private:
	static float Clamp(float f);
	float m_Timestamp;
};

struct S_BoneKeyframe
{
	S_BoneKeyframe() = default;
	S_BoneKeyframe(const glm::mat4& matrix, S_Timestamp timestamp);
	S_BoneKeyframe(const glm::vec3& transformation, const glm::quat& rotation, S_Timestamp timestamp);
	RENDERER_API_EXPORT glm::mat4 GetTransformationMatrix() const;
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
	S_BoneKeyframe Sample(S_Timestamp timestamp) const;
private:
	// ideally BST because intervals do not overlap
	// but we can use vector for now, as we will usually add 
	// keyframes in order
	std::vector<S_BoneKeyframe> m_Timeline;
};


}