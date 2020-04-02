#pragma once

namespace GLEngine::GLRenderer::SpectralRendering {
class C_Spectrum;
class C_MatchingFunction;

class C_DataParser
{
public:
	static C_Spectrum parseSpectrum(const std::filesystem::path& file);
	static C_MatchingFunction parseMatchingFunction(const std::filesystem::path& file);
private:
	static std::vector<std::string_view> splitSVPtr(std::string_view str, std::string_view delims = " ");
	static bool loadFile(const std::filesystem::path& file, std::string& content);
	template<class first, class second>
	static std::vector<std::pair<first, second>> parseFile(const std::filesystem::path& file);
};

}