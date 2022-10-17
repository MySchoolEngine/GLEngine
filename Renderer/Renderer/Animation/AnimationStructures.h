#pragma once

#include <Renderer/RendererApi.h>

#include <Animation/Timestamp.h>

#include <glm/gtc/quaternion.hpp>

namespace GLEngine::Renderer::Animation {

struct S_BoneKeyframe {
	S_BoneKeyframe() = default;
	S_BoneKeyframe(const glm::mat4& matrix, GLEngine::Animation::S_Timestamp timestamp);
	S_BoneKeyframe(const glm::vec3& transformation, const glm::quat& rotation, GLEngine::Animation::S_Timestamp timestamp);
	RENDERER_API_EXPORT glm::mat4	 GetTransformationMatrix() const;
	GLEngine::Animation::S_Timestamp m_Timestamp;
	glm::vec3						 m_Transform;
	glm::quat						 m_Rotation;
};

// todo: move ctor
class C_BoneTimeline {
public:
	C_BoneTimeline() = default;
	explicit C_BoneTimeline(std::size_t numTimestamps);

	void						 AddBoneKeyFrame(std::size_t index, S_BoneKeyframe&& keyframe);
	[[nodiscard]] S_BoneKeyframe Sample(GLEngine::Animation::S_Timestamp timestamp) const;

private:
	// ideally BST because intervals do not overlap
	// but we can use vector for now, as we will usually add
	// keyframes in order
	std::vector<S_BoneKeyframe> m_Timeline;
};

} // namespace GLEngine::Renderer::Animation