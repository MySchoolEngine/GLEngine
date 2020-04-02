#include <GLRendererStdafx.h>

#include <GLRenderer/SpectralRendering/Dataparser.h>

#include <GLRenderer/SpectralRendering/Spectrum.h>
#include <GLRenderer/SpectralRendering/MatchingFunction.h>

namespace GLEngine::GLRenderer::SpectralRendering {
//=================================================================================
C_Spectrum C_DataParser::parseSpectrum(const std::filesystem::path& file)
{
	const auto pairs = C_DataParser::parseFile<int, float>(file);
	C_Spectrum ret(pairs.size());

	for (const auto& pair : pairs)
	{
		ret.AddSample(pair.first, pair.second);
	}

	return ret;
}

//=================================================================================
C_MatchingFunction C_DataParser::parseMatchingFunction(const std::filesystem::path& file)
{
	auto pathX = file;
	auto pathY = file;
	auto pathZ = file;
	pathX.concat("X.mfcie");
	pathY.concat("Y.mfcie");
	pathZ.concat("Z.mfcie");
	const auto pairsX = C_DataParser::parseFile<int, float>(pathX);
	const auto pairsY = C_DataParser::parseFile<int, float>(pathY);
	const auto pairsZ = C_DataParser::parseFile<int, float>(pathZ);

	GL_ASSERT(pairsX.size() == pairsY.size() && pairsY.size() == pairsZ.size(), "We need all the files with same data set.");

	C_MatchingFunction ret(pairsX.size());

	auto ity = pairsY.begin();
	auto itz = pairsZ.begin();


	for (const auto& itx : pairsX)
	{
		ret.AddSample(itx.first, itx.second, ity->second, itz->second);
		++ity;
		++itz;
	}

	return ret;
}

//=================================================================================
// source: https://www.bfilipek.com/2018/07/string-view-perf-followup.html
std::vector<std::string_view> C_DataParser::splitSVPtr(std::string_view str, std::string_view delims)
{
	std::vector<std::string_view> output;
	//output.reserve(str.size() / 2);

	for (auto first = str.data(), second = str.data(), last = first + str.size(); second != last && first != last; first = second + 1) {
		second = std::find_first_of(first, last, std::cbegin(delims), std::cend(delims));

		if (first != second)
			output.emplace_back(first, second - first);
	}

	return output;
}

//=================================================================================
bool C_DataParser::loadFile(const std::filesystem::path& file, std::string& content)
{
	std::ifstream stream(file);

	if (stream.fail())
		return false;

	content = std::string(std::istream_iterator<char>(stream), std::istream_iterator<char>());
	stream.close();
	return true;
}

//=================================================================================
template<class first, class second>
std::vector<std::pair<first, second>> C_DataParser::parseFile(const std::filesystem::path& file)
{
	std::string content;
	loadFile(file, content);

	const auto pairs = splitSVPtr(content, ",");
	std::vector<std::pair<first, second>> ret;
	ret.reserve(pairs.size() / 2);

	for (auto it = pairs.cbegin(); it != pairs.cend(); it += 2)
	{
		std::stringstream waveStream(it->data() + 1);
		std::stringstream intenStream((it + 1)->data());

		first wavelength = first();
		second intensity = second();

		waveStream >> wavelength;
		intenStream >> intensity;

		ret.emplace_back(wavelength, intensity);
	}
	return ret;
}

}