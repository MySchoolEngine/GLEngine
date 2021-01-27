#pragma once

namespace GLEngine::Renderer {

//=================================================================================
inline glm::vec3 UniformSampleHemisphere(const glm::vec2& randoms)
{
	const auto z = randoms[0];
	const auto r = std::sqrt(std::max(0.0f, 1.0f - z * z));
	const auto phi = 2 * glm::pi<float>() * randoms[1];
	return glm::vec3(r * std::cos(phi), r * std::sin(phi), z);
}

//=================================================================================
inline float UniformHemispherePDF()
{
	return 1.0f / glm::two_pi<float>();
}

//=================================================================================
inline glm::vec3 UniformSampleSphere(const glm::vec2& randoms)
{
	const auto z = 1 - 2 * randoms[0];
	const auto r = std::sqrt(std::max(0.0f, 1.0f - z * z));
	const auto phi = 2 * glm::pi<float>() * randoms[1];
	return glm::vec3(r * std::cos(phi), r * std::sin(phi), z);
}

//=================================================================================
inline float UniformSpherePDF()
{
	return 1.0f / (glm::two_pi<float>() * 2.0f);
}
}
