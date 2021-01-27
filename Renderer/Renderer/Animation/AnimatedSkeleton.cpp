#include <RendererStdafx.h>

#include <Renderer/Animation/AnimatedSkeleton.h>
#include <Renderer/Animation/Skeleton.h>


namespace GLEngine::Renderer::Animation {

//=================================================================================
C_AnimatedSkeleton::C_AnimatedSkeleton(std::shared_ptr<C_Skeleotn>&& skeleton)
	: m_Skeleton(skeleton)
	, m_animationTime(0.f)
{
}

//=================================================================================
void C_AnimatedSkeleton::PlayAnimation(const std::shared_ptr<C_SkeletalAnimation>& anim)
{
	m_Animation = anim;
}

//=================================================================================
void C_AnimatedSkeleton::Update(float time)
{
}

} // namespace GLEngine::Renderer::Animation