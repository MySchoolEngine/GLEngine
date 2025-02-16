#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {

struct RENDERER_API_EXPORT S_Joint {
	S_Joint() = default;
	S_Joint(std::size_t index, const std::string& name, const glm::mat4& inverseBindTransform);
	S_Joint(const S_Joint& other);
	S_Joint(S_Joint&& other) noexcept;
	glm::mat4 GetAnimatedTransform() const;

	void ApplyPoseToJoints(std::vector<glm::mat4>& poseData, const glm::mat4& parentTransform) const;

	std::string m_Name;
	// glm::mat4 m_LocalBindTransform; // local bind transform - original
	// transform in relation to parent joint transform
	glm::mat4 m_InverseBindTransform; // in model space
	// todo stack vector.
	std::vector<S_Joint> m_Children;
	std::size_t			 m_Id;
};
} // namespace GLEngine::Renderer