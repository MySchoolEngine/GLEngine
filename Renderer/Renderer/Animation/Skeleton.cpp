#include <RendererStdafx.h>

#include <Renderer/Animation/Skeleton.h>

namespace GLEngine::Renderer::Animation {


//=================================================================================
std::size_t C_Skeleton::GetNumBones() const
{
	return m_NumBones;
}

//=================================================================================
void C_Skeleton::SetNumBonse(std::size_t num)
{
	m_NumBones = num;
}

}