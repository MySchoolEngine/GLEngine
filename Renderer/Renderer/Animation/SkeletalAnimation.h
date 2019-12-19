#pragma once

#include <algorithm>

namespace GLEngine::Animation {
struct S_Timestamp
{
	S_Timestamp() : m_Timestamp(0.f) {}
	S_Timestamp(float timestamp) : m_Timestamp(S_Timestamp::Clamp(timestamp)) {}

private:
	static float Clamp(float f) { return std::clamp(f, 0.0f, 1.0f); }
	float m_Timestamp;
};

class C_SkeletalAnimationKeyframe {


private:
	// should be inbetween 0-1
	S_Timestamp m_Timestamp;
};

class C_SkeletalAnimation {
public:

private:
	std::vector<C_SkeletalAnimationKeyframe> m_Keyframes;
};

}