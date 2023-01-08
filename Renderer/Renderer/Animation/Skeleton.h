#pragma once

#include <Renderer/Animation/Joint.h>
#include <Renderer/RendererApi.h>

#include <glm/gtc/quaternion.hpp>

namespace GLEngine::Renderer {

// represents JointID in whole hierarchy, used for faster queries
struct JointID {
public:
	JointID()
		: m_Name()
		, m_ID(0)
	{}
	explicit JointID(const std::string& name);

	bool operator==(const JointID& other) const;
	bool operator!=(const JointID& other) const;
	bool operator<(const JointID& other) const;

private:
	std::string m_Name;
	std::size_t m_ID;
};

class RENDERER_API_EXPORT C_Skeleton {
public:
	C_Skeleton();
	[[nodiscard]] std::size_t GetNumBones() const;
	void					  SetNumBonse(std::size_t num);
	void					  ApplyPoseToBones(std::vector<glm::mat4>& poseData) const;
	const S_Joint&			  GetRoot() const;

public:
	std::unique_ptr<S_Joint> m_Root;
	std::size_t				 m_NumBones;
};

} // namespace GLEngine::Renderer