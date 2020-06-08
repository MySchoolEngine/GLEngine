#include <GLRendererStdafx.h>

#include <GLRenderer/SpectralRendering/Spectrum.h>

#include <GLRenderer/SpectralRendering/SpectralPlot.h>
#include <GLRenderer/SpectralRendering/MatchingFunction.h>

#include <cmath>

namespace GLEngine::GLRenderer::SpectralRendering {
template<class T>
T map(T x, T in_min, T in_max, T out_min, T out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// C++20 is not that standard yet to use it :(
constexpr float lerp(float a, float b, float t)
{
	return a + t*(b - a);
}

//=================================================================================
C_Spectrum::C_Spectrum(std::size_t expectedSamples /*= 1*/)
{
	m_Samples.reserve(expectedSamples);
}

//=================================================================================
std::size_t C_Spectrum::GetNumSamples() const
{
	return m_Samples.size();
}

//=================================================================================
void C_Spectrum::AddSample(int waveLength, float intensity)
{
	GL_ASSERT(waveLength >= s_LowestWavelen && waveLength <= s_HighestWavelen, "Wavelengths have to be between <{}, {}>", s_LowestWavelen, s_HighestWavelen);
	if (std::find_if(m_Samples.begin(), m_Samples.end(), [&](const auto& it) {return it.first == waveLength;}) == m_Samples.end())
		m_Samples.emplace_back(waveLength, intensity);
}

//=================================================================================
void C_Spectrum::FillData(C_SpectralPlot& plot, std::size_t line) const
{
	GL_ASSERT(plot.m_yData.size() > line, "");
	plot.m_xData.clear();
	plot.m_yData[line].clear();

	std::transform(m_Samples.begin(), m_Samples.end(), std::back_inserter(plot.m_xData), [](const auto& it) {
		return static_cast<float>(it.first);
		});
	std::transform(m_Samples.begin(), m_Samples.end(), std::back_inserter(plot.m_yData[line]), [](const auto& it) {
		return it.second;
		});
}

//=================================================================================
float C_Spectrum::Sample(int wavelength) const
{
	GL_ASSERT(wavelength >= s_LowestWavelen && wavelength <= s_HighestWavelen, "Wavelengths have to be between <{}, {}>", s_LowestWavelen, s_HighestWavelen);
	const auto higher = std::find_if(m_Samples.begin(), m_Samples.end(), [&](const auto& it) { return it.first > wavelength; });
	if (higher == m_Samples.end())
	{
		return m_Samples.back().second;
	}
	else if (higher == m_Samples.begin())
	{
		return higher->second;
	}
	else
	{
		const auto lower = higher - 1;
		return lerp(lower->second, higher->second, map<float>(static_cast<float>(wavelength), static_cast<float>(lower->first), static_cast<float>(higher->first), 0.0f, 1.0f));
	}
}

//=================================================================================
C_Spectrum C_Spectrum::SampleUniformly(std::size_t samples) const
{
	C_Spectrum ret(samples);

	for (int i = 1; i <= samples; ++i)
	{
		int wavelength = static_cast<int>(lerp(s_LowestWavelen, s_HighestWavelen, static_cast<float>(i) / static_cast<float>(samples)));
		float intensity = Sample(wavelength);
		ret.AddSample(wavelength, intensity);
	}

	return ret;
}

//=================================================================================
C_Spectrum C_Spectrum::SampleRandomly(std::size_t samples) const
{
	C_Spectrum ret(samples);

	unsigned seed1 = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
	std::default_random_engine generator(seed1);
	std::uniform_int_distribution<int> distribution(s_LowestWavelen, s_HighestWavelen);

	while (ret.GetNumSamples()<samples)
	{
		int wavelength = distribution(generator);
		float intensity = Sample(wavelength);
		ret.AddSample(wavelength, intensity);
	}

	ret.SortData();
	return ret;
}

//=================================================================================
C_Spectrum C_Spectrum::GetSameSamplig(const C_Spectrum& pattern) const
{
	C_Spectrum ret(pattern.m_Samples.size());

	for (const auto it : pattern.m_Samples)
	{
		ret.AddSample(it.first, Sample(it.first));
	}

	return ret;
}

//=================================================================================
void C_Spectrum::SortData()
{
	std::sort(m_Samples.begin(), m_Samples.end(), [](const auto& it, const auto& second) {return it.first < second.first; });
}

//=================================================================================
C_Spectrum C_Spectrum::SampleHero(std::size_t samples) const
{
	unsigned seed1 = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
	std::default_random_engine generator(seed1);
	std::uniform_int_distribution<int> distribution(s_LowestWavelen, s_HighestWavelen);

	const int heroWavelength = distribution(generator);

	const auto delta = static_cast<int>(std::ceil(std::max((float(s_HighestWavelen - s_LowestWavelen)) / samples, 1.0f)));

	C_Spectrum ret(samples);

	for (int i = 0; i < samples; ++i)
	{
		const int sampledWavelen = (((heroWavelength + delta*i) - s_LowestWavelen) % (s_HighestWavelen - s_LowestWavelen)) + s_LowestWavelen;

		ret.AddSample(sampledWavelen, Sample(sampledWavelen));
	}

	ret.SortData();

	return ret;
}

//=================================================================================
C_Spectrum C_Spectrum::operator*(const C_Spectrum& other) const
{
	GL_ASSERT(GetNumSamples() == other.GetNumSamples(), "Both spectra needs same number of samples");

	C_Spectrum ret(GetNumSamples());
	for (int i = 0; i < GetNumSamples(); ++i)
	{
		GL_ASSERT(m_Samples[i].first == other.m_Samples[i].first, "Sampled wavelengths doesn't match");

		ret.AddSample(m_Samples[i].first, m_Samples[i].second * other.m_Samples[i].second);
	}

	return ret;
}

//=================================================================================
float C_Spectrum::Integrate() const
{
	auto sum = 0.f;

	for (auto it = m_Samples.begin() + 1; it != m_Samples.end(); ++it) {
		auto dx = std::abs(((it - 1)->first - it->first));
		auto h = ((it - 1)->second + (*it).second) * 0.5f;
		sum += dx * h;
	}

	return sum;
}

//=================================================================================
glm::vec3 C_Spectrum::GetXYZ(const C_MatchingFunction& mf) const
{
	const auto X = (*this * mf.m_Samples[0].GetSameSamplig(*this)).Integrate();
	const auto Y = (*this * mf.m_Samples[1].GetSameSamplig(*this)).Integrate();
	const auto Z = (*this * mf.m_Samples[2].GetSameSamplig(*this)).Integrate();

	glm::vec3 xyz(X, Y, Z);
	return xyz;
}

}