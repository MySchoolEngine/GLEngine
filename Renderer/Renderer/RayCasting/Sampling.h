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

//=================================================================================
// directly taken from Doc. Jaroslav Køivánek codebase
inline glm::vec2 SampleConcentricDisc(const glm::vec2& aSamples)
{
	float phi, r;

	const auto a = 2 * aSamples.x - 1;   /* (a,b) is now on [-1,1]^2 */
	const auto b = 2 * aSamples.y - 1;

	if (a > -b)      /* region 1 or 2 */
	{
		if (a > b)   /* region 1, also |a| > |b| */
		{
			r = a;
			phi = (glm::pi<float>() / 4.f) * (b / a);
		}
		else        /* region 2, also |b| > |a| */
		{
			r = b;
			phi = (glm::pi<float>() / 4.f) * (2.f - (a / b));
		}
	}
	else            /* region 3 or 4 */
	{
		if (a < b)   /* region 3, also |a| >= |b|, a != 0 */
		{
			r = -a;
			phi = (glm::pi<float>() / 4.f) * (4.f + (b / a));
		}
		else        /* region 4, |b| >= |a|, but a==0 and b==0 could occur. */
		{
			r = -b;

			if (b != 0)
				phi = (glm::pi<float>() / 4.f) * (6.f - (a / b));
			else
				phi = 0;
		}
	}

	glm::vec2 res;
	res.x = r * std::cos(phi);
	res.y = r * std::sin(phi);
	return res;
}

//=================================================================================
inline glm::vec3 CosineSampleHemisphere(const glm::vec2& rng)
{
	const auto d = SampleConcentricDisc(rng);
	const auto z = std::sqrt(std::max(0.f, 1.f - d.x * d.x - d.y * d.y));
	return glm::vec3(d.x, z, d.y);
}

//=================================================================================
inline float CosineSampleHemispherePDF(const float cosTheta)
{
	return cosTheta * glm::one_over_pi<float>();
}
}
