#include <RendererStdAfx.h>

#include <Renderer/Animation/Joint.h>

namespace GLEngine::Renderer::Animation {

//=================================================================================
S_Joint::S_Joint(std::size_t index, std::string name, glm::mat4 localBindTransform) : m_Id(index)
, m_Name(name)
, m_LocalBindTransform(localBindTransform)
, m_InverseBindTransfomr(1.f)
{

}

//=================================================================================
glm::mat4 S_Joint::GetAnimatedTransform() const
{
	//auto mat = glm::translate(glm::mat4(1.f), m_LocalBindTransform);
	// TODO!!!
	return glm::inverse(m_InverseBindTransfomr);
}

//=================================================================================
void S_Joint::CalcInverseBindTransfomr(const glm::mat4& parentBindTransform)
{
	const auto modelBindTransform = parentBindTransform * m_LocalBindTransform;
	m_InverseBindTransfomr = glm::inverse(modelBindTransform);
	for (auto& child : m_Children)
	{
		child.CalcInverseBindTransfomr(modelBindTransform);
	}
}

}
