#include <RendererStdAfx.h>

#include <Renderer/Animation/Joint.h>

namespace GLEngine::Renderer::Animation {

//=================================================================================
S_Joint::S_Joint(std::size_t index, std::string name, glm::mat4 localBindTransform) 
	: m_Id(index)
	, m_Name(name)
	, m_InverseBindTransfomr(localBindTransform)
{

}

//=================================================================================
S_Joint::S_Joint(const S_Joint& other)
	: m_Name(other.m_Name)
	, m_InverseBindTransfomr(other.m_InverseBindTransfomr)
	, m_Id(other.m_Id)
	, m_Children(other.m_Children)
{
	CORE_LOG(E_Level::Info, E_Context::Render, "Copying bone: {}", m_Name);
}

//=================================================================================
S_Joint::S_Joint(S_Joint&& other)
	: m_Name(std::move(other.m_Name))
	, m_InverseBindTransfomr(std::move(other.m_InverseBindTransfomr))
	, m_Id(other.m_Id)
	, m_Children(std::move(other.m_Children))
{
	CORE_LOG(E_Level::Info, E_Context::Render, "Movin' bone: {}", m_Name);
}

//=================================================================================
glm::mat4 S_Joint::GetAnimatedTransform() const
{
	//auto mat = glm::translate(glm::mat4(1.f), m_LocalBindTransform);
	// TODO!!!
	return glm::inverse(m_InverseBindTransfomr);
}

}
