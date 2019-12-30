#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer::Animation {

struct RENDERER_API_EXPORT S_Joint {
	S_Joint() = default;
	S_Joint(std::size_t index, std::string name, glm::mat4 inverseBindTransform);
	S_Joint(const S_Joint& other);
	S_Joint(S_Joint&& other);
	glm::mat4 GetAnimatedTransform() const;


	std::string m_Name;
	std::size_t m_Id;
	// glm::mat4 m_LocalBindTransform; // local bind transform - original transform in relation to parent joint transform
	glm::mat4 m_InverseBindTransfomr; // in model space
	// todo stack vector.
	std::vector<S_Joint> m_Children;
};
}