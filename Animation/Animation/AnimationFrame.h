#pragma once

#include <Animation/Timestamp.h>

namespace GLEngine::Animation {
template<class valT>
struct S_AnimationFrame {
public:
	valT		Value;
	S_Timestamp Time;
};
} // namespace GLEngine::Animation
