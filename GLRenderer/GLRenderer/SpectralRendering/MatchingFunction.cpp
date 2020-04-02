#include <GLRendererStdafx.h>

#include <GLRenderer/SpectralRendering/MatchingFunction.h>

#include <GLRenderer/SpectralRendering/SpectralPlot.h>

namespace GLEngine::GLRenderer::SpectralRendering {

//=================================================================================
C_MatchingFunction::C_MatchingFunction(std::size_t expectedSamples /*= 1*/)
{
	m_Samples[0] = C_Spectrum(expectedSamples);
	m_Samples[1] = C_Spectrum(expectedSamples);
	m_Samples[2] = C_Spectrum(expectedSamples);
}

//=================================================================================
std::size_t C_MatchingFunction::GetNumSamples() const
{
	return m_Samples.size();
}

//=================================================================================
void C_MatchingFunction::AddSample(int waveLength, float X, float Y, float Z)
{
	m_Samples[0].AddSample(waveLength, X);
	m_Samples[1].AddSample(waveLength, Y);
	m_Samples[2].AddSample(waveLength, Z);
}

//=================================================================================
void C_MatchingFunction::FillData(C_SpectralPlot& plot) const
{
	plot.m_xData.clear();
	plot.m_yData.resize(3);

	m_Samples[0].FillData(plot, 0);
	m_Samples[1].FillData(plot, 1);
	m_Samples[2].FillData(plot, 2);
}
}