#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/ShaderPreprocessor.h>

namespace GLEngine {
namespace GLRenderer {
namespace Shaders {

//=================================================================================
const std::regex C_ShaderPreprocessor::s_IncludeFileName = std::regex(R"(^(#include )\"([^\"]*)\"$)");
const std::regex C_ShaderPreprocessor::s_DefineRegEx			= std::regex(R"(^(#define )([^\s]*)\s([^\s]+)$)");

//=================================================================================
void C_ShaderPreprocessor::Define(const std::string& symbol, const std::string& value) {
	m_defines[symbol] = value;
}

//=================================================================================
std::string C_ShaderPreprocessor::PreprocessFile(const std::string& src, const std::string& filepath)
{
	std::string ret = src;
	IncludesFiles(ret, filepath);
	GetDefines(ret);
	ReplaceConstants(ret);

	return ret;
}

//=================================================================================
void C_ShaderPreprocessor::IncludesFiles(std::string& content, const std::string& filepath) {
	std::smatch m;
	std::string result = "";

	while (std::regex_search(content, m, s_IncludeFileName)) {
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
	std::smatch m;
	
	std::string result = "";
	
	while (std::regex_search(content, m, s_DefineRegEx)) {
		result += m.prefix().str();
		Define(m[2], m[3]);
		content = m.suffix().str();
	}
	content = result + content;
}

//=================================================================================
void C_ShaderPreprocessor::ReplaceConstants(std::string& content) {
	for (const auto& define : m_defines) {
		size_t index = 0;
		while (true) {
			index = content.find(define.first, index);
			if (index == std::string::npos) break;

			/* Make the replacement. */
			content.replace(index, define.first.length(), define.second);

			/* Advance index forward so the next iteration doesn't pick it up as well. */
			index += define.first.length();
		}
	}
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
}
}
}