#include <RendererStdafx.h>

#include <Renderer/Shaders/ShaderPreprocessor.h>

namespace GLEngine::Renderer::Shaders {

//=================================================================================
const std::regex C_ShaderPreprocessor::s_IncludeFileName   = std::regex(R"(^(#include )\"([^\"]*)\"$)");
const std::regex C_ShaderPreprocessor::s_GenerateStruct	   = std::regex(R"(^(@struct )([^\"\n;\s]*))");
const std::regex C_ShaderPreprocessor::s_DefineRegEx	   = std::regex(R"(^(#define )([^\s]*)\s([^\s]+)$)");
const std::regex C_ShaderPreprocessor::s_IfDefinedRegEx	   = std::regex(R"(^(#if )(!?)(defined)\s([^\s]+)$)");
const std::regex C_ShaderPreprocessor::s_EndIfDefinedRegEx = std::regex(R"(^#endif([\s]*\/\/.*)?$)");

//=================================================================================
C_ShaderPreprocessor::C_ShaderPreprocessor(std::unique_ptr<I_CodeProvider>&& codeProvider)
	: m_CodeProvider(std::move(codeProvider))
{
}

//=================================================================================
C_ShaderPreprocessor::~C_ShaderPreprocessor() = default;

//=================================================================================
void C_ShaderPreprocessor::Define(const std::string& symbol, const std::string& value)
{
	m_Defines[symbol] = value;
}

//=================================================================================
bool C_ShaderPreprocessor::IsDefined(const std::string& name) const
{
	return m_Defines.contains(name);
}

//=================================================================================
std::string C_ShaderPreprocessor::PreprocessFile(const std::string& src, const std::filesystem::path& filepath)
{
	//@todo if statements works only with constants defined from outside! FIX ME
	std::string ret = src;
	IncludesFiles(ret, filepath);
	ResolveIfStatements(ret);
	GetDefines(ret);
	ReplaceConstants(ret);

	if (m_CodeProvider)
		CodeGeneration(ret);

	return ret;
}

//=================================================================================
C_ShaderPreprocessor::T_Paths C_ShaderPreprocessor::GetTouchedPaths() const
{
	return m_Paths;
}

//=================================================================================
void C_ShaderPreprocessor::IncludesFiles(std::string& content, const std::filesystem::path& folderPath)
{
	std::smatch m;
	std::string result = "";

	while (std::regex_search(content, m, s_IncludeFileName))
	{
		result += m.prefix().str();
		std::string file;
		const auto	filePath = folderPath / std::filesystem::path(m[2].str());
		if (_loadFile(filePath, file))
		{
			result += file;
			m_Paths.emplace_back(filePath);
		}
		else
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Failed to open included file: {}\n", filePath);
			m_Result = false;
		}
		content = m.suffix().str();
	}
	content = result + content;
}

//=================================================================================
void C_ShaderPreprocessor::CodeGeneration(std::string& content) const
{
	std::smatch m;
	std::string result;

	while (std::regex_search(content, m, s_GenerateStruct))
	{
		result += m.prefix().str();
		result += m_CodeProvider->GetStructCode(m[2]);
		content = m.suffix().str();
	}
	content = result + content;
}

//=================================================================================
void C_ShaderPreprocessor::GetDefines(std::string& content)
{
	std::smatch m;

	std::string result;

	while (std::regex_search(content, m, s_DefineRegEx))
	{
		result += m.prefix().str();
		Define(m[2], m[3]);
		content = m.suffix().str();
	}
	content = result + content;
}

//=================================================================================
void C_ShaderPreprocessor::ReplaceConstants(std::string& content) const
{
	for (const auto& define : m_Defines)
	{
		size_t index = 0;
		while (true)
		{
			index = content.find(define.first, index);
			if (index == std::string::npos)
				break;

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

//=================================================================================
void C_ShaderPreprocessor::ResolveIfStatements(std::string& content) const
{
	std::smatch m;
	std::string result;

	while (std::regex_search(content, m, s_IfDefinedRegEx))
	{
		result += m.prefix().str();
		std::smatch endIfMatch;
		std::string rest(m.suffix().str());
		if (!std::regex_search(rest, endIfMatch, s_EndIfDefinedRegEx))
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "No matching #endif for given #if statement.");

			content = m.suffix().str();
			continue;
		}

		const auto negation = (m[2].length() == 0);
		if (IsDefined(m[4]) == negation)
		{
			// look for next endif that's obviously bug
			result += endIfMatch.prefix().str();
		}
		content = endIfMatch.suffix().str();
	}
	content = result + content;
}

} // namespace GLEngine::Renderer::Shaders