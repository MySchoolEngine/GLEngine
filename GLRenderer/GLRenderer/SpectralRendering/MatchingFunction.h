#pragma once

#include <GLRenderer/SpectralRendering/Spectrum.h>

namespace GLEngine::GLRenderer::SpectralRendering {

class C_SpectralPlot;

class C_MatchingFunction
{
public:
	C_MatchingFunction(std::size_t expectedSamples = 1);

	std::size_t GetNumSamples() const;

	// I support just integer wavelengths 
	void AddSample(int waveLength, float X, float Y, float Z);

	void FillData(C_SpectralPlot& plot) const;
protected:
	std::array<C_Spectrum, 3> m_Samples;
};
}