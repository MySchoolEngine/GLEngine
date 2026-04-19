#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {

// represents JointID in whole hierarchy, used for faster queries
struct JointID {
public:
	JointID()
		: m_Name()
		, m_ID(0)
	{
	}
	explicit JointID(const std::string& name);

	bool operator==(const JointID& other) const;
	bool operator!=(const JointID& other) const;
	bool operator<(const JointID& other) const;

	const std::string& GetName() const { return m_Name; }

private:
	std::string m_Name;
	std::size_t m_ID;
};

using T_BoneIndex								   = std::uint16_t;
inline constexpr static T_BoneIndex s_BoneBadIndex = (T_BoneIndex)-1;

struct RENDERER_API_EXPORT S_Joint {
	S_Joint() = default;
	S_Joint(const std::string& name, glm::mat4 inverseBindTransform);
	S_Joint(const S_Joint& other);
	S_Joint(S_Joint&& other) noexcept;
	S_Joint&  operator=(const S_Joint& other) = default;
	glm::mat4 GetAnimatedTransform() const;

	// glm::mat4 m_LocalBindTransform; // local bind transform - original
	// transform in relation to parent joint transform
	glm::mat4				 m_InverseBindTransform; // in model space
	glm::mat4				 m_InverseLocalBindTransform;
	JointID					 m_Id;
	T_BoneIndex				 m_Parent = s_BoneBadIndex;
	std::vector<T_BoneIndex> m_Children;
};
} // namespace GLEngine::Renderer