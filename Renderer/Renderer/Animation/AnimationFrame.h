#pragma once

#include <Renderer/Animation/Timestamp.h>

namespace GLEngine::Renderer {
template<class valT>
struct S_AnimationFrame {
public:
	valT		Value;
	S_Timestamp Time;
};
} // namespace GLEngine::Animation
