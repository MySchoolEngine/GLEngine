#pragma once

namespace GLEngine::GUI {

//=================================================================================
template<int samples>
C_PlotLine<samples>::C_PlotLine(std::string&& name)
	: m_Samples()
	, m_Name(std::move(name))
	, m_SampleID(0)
{
	m_Samples.fill(0.0f);
}

//=================================================================================
template<int samples>
void C_PlotLine<samples>::Draw() const
{
	int samplesNum = samples;
	if (m_SampleID < samples) {
		samplesNum = m_SampleID;
	}
	::ImGui::PlotLines(m_Name.c_str(), m_Samples.data(), samplesNum);
}

//=================================================================================
template<int samples>
void C_PlotLine<samples>::Sample(float sample)
{
	m_Samples[m_SampleID++%samples] = sample;
}

//=================================================================================
template<int samples>
float C_PlotLine<samples>::Avg() const
{
	const auto allSamples = std::accumulate(m_Samples.begin(), m_Samples.end(), 0.0f);
	if (m_SampleID < samples) {
		return allSamples / m_SampleID;
	}

	return allSamples / samples;
}

//=================================================================================
template<int samples>
float C_PlotLine<samples>::LastSample() const
{
	const auto actualSample = m_SampleID%samples;
	if (actualSample == 0) {
		return *m_Samples.end();
	}
	return m_Samples[actualSample];
}

//=================================================================================
template<int samples>
typename C_PlotLine<samples>::T_SamplesCIter C_PlotLine<samples>::cbegin() const
{
	return m_Samples.cbegin();
}

//=================================================================================
template<int samples>
typename C_PlotLine<samples>::T_SamplesCIter C_PlotLine<samples>::cend() const
{
	return m_Samples.cend();
}

}