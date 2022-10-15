#pragma once

namespace GLEngine::Animation {

[[nodiscard]] inline float Linear(const float t)
{
	return t;
}

[[nodiscard]] inline float QuadraticEaseIn(const float t)
{
	return t * t;
}

[[nodiscard]] inline float QuadraticEaseOut(const float t)
{
	return -(t * (t - 2));
}

[[nodiscard]] inline float CubicEaseIn(const float t)
{
	return t * t * t;
}

[[nodiscard]] inline float CubicEaseOut(const float t)
{
	float f = (t - 1);
	return f * f * f + 1;
}

}

