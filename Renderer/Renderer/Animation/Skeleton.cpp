#include <RendererStdafx.h>

#include <Renderer/Animation/Skeleton.h>

namespace GLEngine::Renderer::Animation {

//=================================================================================
void C_Skeleton::InitializeSkeleton()
{
	m_Root->CalcInverseBindTransfomr(glm::mat4(1.f));
}

}