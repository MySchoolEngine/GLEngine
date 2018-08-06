#include "ShaderPreprocessor.h"

#include <fstream>
#include <regex>
#include <algorithm>
#include <iostream>
#include <exception>

void C_ShaderPreprocessor::Define(const std::string& symbol, const std::string& value) {
	m_defines[symbol] = value;
}

//=================================================================================
std::string C_ShaderPreprocessor::PreprocessFile(const std::string& src, const std::string& filepath)
{
	std::string ret = src;
	IncludesFiles(ret, filepath);
	//GetDefines(conent);
	//ReplaceConstants(ReplaceConstants);

	return ret;
}

//=================================================================================
void C_ShaderPreprocessor::IncludesFiles(std::string& content, const std::string& filepath) {
	std::smatch m;
	std::regex e(R"(^(#include )\"([^\"]*)\"$)");

	std::string result = "";

	while (std::regex_search(content, m, e)) {
		result += m.prefix().str();
		std::string file;
		if (_loadFile((filepath + m[2].str()).c_str(), file)) {
			result += file;
		}
		else {
			throw std::exception(("Failed to open included file: " + filepath + m[2].str() + "\n").c_str());
		}
		content = m.suffix().str();
	}
	content = result + content;
}

//=================================================================================
void C_ShaderPreprocessor::GetDefines(std::string& content) {
	//std::smatch m;
	//std::regex e("^(#define )([^\s]*)\s([^\s]+)$");
	//
	//std::string result = "";
	//
	//while (std::regex_search(content, m, e)) {
	//	result += m.prefix().str();
	//	Define(m[1], m[2]);
	//	result += m.suffix().str();
	//	//s = m.suffix().str();
	//}
}

//=================================================================================
void C_ShaderPreprocessor::ReplaceConstants(std::string& content) {
	//for (const auto& define : m_defines) {
	//	std::replace(content.begin(), content.end(), define.first, define.second);
	//}
}

//=================================================================================
// taken from Ing. Josef Kobrtek's sources
bool C_ShaderPreprocessor::_loadFile(const char* file, std::string& content)
{
	std::ifstream stream(file);

	if (stream.fail())
		return false;

	content = std::string(std::istream_iterator<char>(stream >> std::noskipws), std::istream_iterator<char>());
	return true;
}