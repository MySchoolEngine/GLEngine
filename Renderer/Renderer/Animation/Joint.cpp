#include <RendererStdafx.h>

#include <Renderer/Animation/Joint.h>

namespace GLEngine::Renderer {

//=================================================================================
S_Joint::S_Joint(const std::string& name, glm::mat4 localBindTransform)
	: m_Id(name)
	, m_InverseLocalBindTransform(glm::mat4(1.f))
	, m_InverseBindTransform(localBindTransform)
{
}

//=================================================================================
S_Joint::S_Joint(const S_Joint& other)
	: m_InverseBindTransform(other.m_InverseBindTransform)
	, m_InverseLocalBindTransform(other.m_InverseLocalBindTransform)
	, m_Id(other.m_Id)
	, m_Children(other.m_Children)
	, m_Id(other.m_Id)
{
	CORE_LOG(E_Level::Info, E_Context::Render, "Copying bone: {}", m_Id.GetName());
}

//=================================================================================
S_Joint::S_Joint(S_Joint&& other) noexcept
	: m_InverseBindTransform(std::move(other.m_InverseBindTransform))
	, m_InverseLocalBindTransform(std::move(other.m_InverseLocalBindTransform))
	, m_Id(other.m_Id)
	, m_Children(std::move(other.m_Children))
	, m_Id(other.m_Id)
{
	CORE_LOG(E_Level::Info, E_Context::Render, "Movin' bone: {}", m_Id.GetName());
}

//=================================================================================
glm::mat4 S_Joint::GetAnimatedTransform() const
{
	// auto mat = glm::translate(glm::mat4(1.f), m_LocalBindTransform);
	// TODO!!!
	return glm::inverse(m_InverseBindTransform);
}

//=================================================================================
// JointID
//=================================================================================
JointID::JointID(const std::string& name)
	: m_Name(name)
	, m_ID(std::hash<std::string>{}(name))
{
}

//=================================================================================
bool JointID::operator==(const JointID& other) const
{
	return m_ID == other.m_ID;
}

//=================================================================================
bool JointID::operator!=(const JointID& other) const
{
	return m_ID != other.m_ID;
}

//=================================================================================
bool JointID::operator<(const JointID& other) const
{
	return m_ID < other.m_ID;
}

} // namespace GLEngine::Renderer
