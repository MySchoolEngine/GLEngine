#pragma once

#include <Renderer/Animation/SkeletalAnimation.h>

namespace GLEngine::Renderer::Animation {

class C_Skeleotn;

// this class should hold not owning ptr to skeleton,
// take care of what animation is currently played, in the future also
// hold "skeleton shader" etc
class C_AnimatedSkeleton {
public:
	explicit C_AnimatedSkeleton(std::shared_ptr<C_Skeleotn>&& skeleton);

	void PlayAnimation(const std::shared_ptr<C_SkeletalAnimation>& anim);
	void Update(float time);

private:
	std::shared_ptr<C_Skeleotn>			 m_Skeleton;
	std::shared_ptr<C_SkeletalAnimation> m_Animation;
	S_Timestamp							 m_animationTime;
};

} // namespace GLEngine::Renderer::Animation