#pragma once

#include <Animation/AnimationApi.h>

namespace GLEngine::Animation {

/**
 * Clamps into the interval [0,1]
 */
struct ANIMATION_API_EXPORT S_Timestamp {
	S_Timestamp();
	S_Timestamp(float timestamp);

	[[nodiscard]] bool		  operator>=(const S_Timestamp& other) const;
	[[nodiscard]] bool		  operator>(const S_Timestamp& other) const;
	[[nodiscard]] bool		  operator<(const S_Timestamp& other) const;
	[[nodiscard]] bool		  operator<=(const S_Timestamp& other) const;
	[[nodiscard]] S_Timestamp operator-(const S_Timestamp& rhs) const;
	[[nodiscard]] S_Timestamp operator+(const S_Timestamp& rhs) const;
	[[nodiscard]] float		  GetValue() const { return m_Timestamp; }

private:
	static float Clamp(float f);
	float		 m_Timestamp;
};

} // namespace GLEngine::Animation