#pragma once


namespace GLEngine::Renderer::Animation {

struct API_EXPORT S_Joint {
	S_Joint(std::size_t index, std::string name, glm::mat4 localBindTransform);
	glm::mat4 GetAnimatedTransform() const;
	void CalcInverseBindTransfomr(const glm::mat4& parentBindTransform);


	std::string m_Name;
	std::size_t m_Id;
	glm::mat4 m_LocalBindTransform; // local bind transform - original transform in relation to parent joint transform
	glm::mat4 m_InverseBindTransfomr; // in model space
	// todo stack vector.
	std::vector<S_Joint> m_Children;
};
}