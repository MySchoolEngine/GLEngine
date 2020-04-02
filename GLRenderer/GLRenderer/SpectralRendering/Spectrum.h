#pragma once

namespace GLEngine::GLRenderer::SpectralRendering {

class C_SpectralPlot;
class C_MatchingFunction;


class C_Spectrum
{
public:
	C_Spectrum(std::size_t expectedSamples = 1);

	std::size_t GetNumSamples() const;

	// I support just integer wavelengths 
	void AddSample(int waveLength, float intensity);

	void FillData(C_SpectralPlot& plot, std::size_t line) const;

	glm::vec3 GetXYZ(const C_MatchingFunction& mf) const;

	float Integrate() const;

	C_Spectrum SampleUniformly(std::size_t samples) const;
	C_Spectrum SampleRandomly(std::size_t samples) const;
	C_Spectrum SampleHero(std::size_t samples) const;

	C_Spectrum GetSameSamplig(const C_Spectrum& pattern) const;

	float Sample(int wavelength) const;

	C_Spectrum operator*(const C_Spectrum& other) const;
protected:
	void SortData();

	static constexpr int s_LowestWavelen = 380;
	static constexpr int s_HighestWavelen = 780;
	std::vector<std::pair<int, float>> m_Samples;
};
}