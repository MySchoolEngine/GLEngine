#include <RendererStdafx.h>

#include <Renderer/Shaders/ShaderPreprocessor.h>

#include <fmt/format.h>

namespace GLEngine::Renderer::Shaders {

//=================================================================================
const std::regex C_ShaderPreprocessor::s_IncludeFileName	= std::regex(R"(^(#include )\"([^\"]*)\"$)");
const std::regex C_ShaderPreprocessor::s_GenerateStruct		= std::regex(R"(^(@struct )([^\"\n;\s]*))");
const std::regex C_ShaderPreprocessor::s_DefineRegEx		= std::regex(R"(^(#define )([^\s]*)\s([^\s]+)$)");

//=================================================================================
C_ShaderPreprocessor::C_ShaderPreprocessor(std::unique_ptr<I_CodeProvider>&& codeProvider)
	: m_CodeProvider(std::move(codeProvider))
{

}

//=================================================================================
C_ShaderPreprocessor::~C_ShaderPreprocessor() = default;

//=================================================================================
void C_ShaderPreprocessor::Define(const std::string& symbol, const std::string& value) {
	m_defines[symbol] = value;
}

//=================================================================================
std::string C_ShaderPreprocessor::PreprocessFile(const std::string& src, const std::filesystem::path& filepath)
{
	std::string ret = src;
	IncludesFiles(ret, filepath);
	GetDefines(ret);
	ReplaceConstants(ret);

	CodeGeneration(ret);

	return ret;
}

//=================================================================================
C_ShaderPreprocessor::T_Paths C_ShaderPreprocessor::GetTouchedPaths() const
{
	return m_Paths;
}

//=================================================================================
void C_ShaderPreprocessor::IncludesFiles(std::string& content, const std::filesystem::path& folderPath) {
	std::smatch m;
	std::string result = "";

	while (std::regex_search(content, m, s_IncludeFileName)) {
		result += m.prefix().str();
		std::string file;
		const auto filePath = folderPath / std::filesystem::path(m[2].str());
		if (_loadFile(filePath, file)) {
			result += file;
			m_Paths.emplace_back(filePath);
		}
		else {
			CORE_LOG(E_Level::Error, E_Context::Render, "Failed to open included file: {}\n", filePath);
			m_Result = false;
		}
		content = m.suffix().str();
	}
	content = result + content;
}

//=================================================================================
void C_ShaderPreprocessor::CodeGeneration(std::string& content)
{
	std::smatch m;
	std::string result = "";

	while (std::regex_search(content, m, s_GenerateStruct)) {
		result += m.prefix().str();
		result += m_CodeProvider->GetStructCode(m[2]);
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
			index += define.second.length();
		}
	}
}

//=================================================================================
// taken from Ing. Josef Kobrtek's sources
bool C_ShaderPreprocessor::_loadFile(const std::filesystem::path& file, std::string& content)
{
	std::ifstream stream(file);

	if (stream.fail())
		return false;

	content = std::string(std::istream_iterator<char>(stream >> std::noskipws), std::istream_iterator<char>());
	return true;
}

}